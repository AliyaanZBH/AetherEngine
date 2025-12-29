#pragma once
//===============================================================================
// desc: A collection of handy debug utilies for DirectX/WinAPI, chiefly the ability to get better debugging info from HResults!
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Shader.h"
#include "Vertex.h"
//===============================================================================


static std::wstring ToWide(const std::string& str)
{
	int sizeNeeded = MultiByteToWideChar(
		CP_UTF8, 0,
		str.c_str(), (int)str.size(),
		nullptr, 0);

	std::wstring wstr(sizeNeeded, 0);

	MultiByteToWideChar(
		CP_UTF8, 0,
		str.c_str(), (int)str.size(),
		&wstr[0], sizeNeeded);

	return wstr;
}


namespace Aether
{
	static std::string ResolveDirectXShaderPath(const std::string& name)
	{
		return name + ".hlsl";
	}

	static const char* ToDirectXSemantic(eShaderSemantic semantic)
	{
		switch (semantic)
		{
		case eShaderSemantic::kPosition:	return "POSITION";
		case eShaderSemantic::kNormal:		return "NORMAL";
		case eShaderSemantic::kColour:		return "COLOR";
		case eShaderSemantic::kTexCoord0:	return "TEXCOORD";
		case eShaderSemantic::kTexCoord1:	return "TEXCOORD";
		case eShaderSemantic::kTangent:		return "TANGENT";
		case eShaderSemantic::kBitangent:	return "BINORMAL";
		default:
			AETHER_ASSERT(false, "Unsupported semantic");
			return "";
		}
	}

	static const char* ShaderStageToHLSLCompilerString(eShaderStage stage)
	{
		switch (stage)
		{
			case eShaderStage::kVertex: return "vs_5_0";
			case eShaderStage::kPixel: return "ps_5_0";
			case eShaderStage::kCompute: return "cs_5_0";
		}

		AETHER_ASSERT(AETHER_FAIL, "Unknown shader stage for DirectX");
		return "";
	}
	
	static DXGI_FORMAT ToDXGIFormat(eVertexAttributeFormat fmt)
	{
		switch (fmt)
		{
			case eVertexAttributeFormat::kFloat3: return DXGI_FORMAT_R32G32B32_FLOAT;
			case eVertexAttributeFormat::kFloat4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
	
		AETHER_ASSERT(false, "Unknown vertex format for DirectX");
		return DXGI_FORMAT_UNKNOWN;
	}
};

// [AZB]: I didn't want to have to keep copy and pasting shader files into all the different directories, so I wrote this in order to get shader files from a single common folder
class CustomIncludeHandler : public ID3DInclude
{
public:

	// [AZB]: Constructor that accepts a list of directories
	CustomIncludeHandler(const std::vector<std::wstring>& includeDirs, std::wstring currentShader)
		: m_IncludeDirs(includeDirs)
	{
		// [AZB]: Add the current shader file to the search paths.
		int current = currentShader.find_last_of(L"\\", 0);
		m_IncludeDirs.push_back(currentShader);

		// [AZB]:  Add current working directory to the search paths, mimicking the existing behaviour of D3D_COMPILE_STANDARD_FILE_INCLUDE
		wchar_t currentDir[MAX_PATH];
		GetCurrentDirectoryW(MAX_PATH, currentDir);
		m_IncludeDirs.push_back(currentDir);

		// [AZB]: Also append the shader folder that is situated within the working directory
		std::wstring currentShaderDir = currentDir;
		currentShaderDir += L"\\Shaders";
		m_IncludeDirs.push_back(currentShaderDir);

	}

	// [AZB]: Override Open method to load a files from my designated include directories
	STDMETHOD(Open)(THIS_ D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override
	{
		// [AZB]: Try and find the file from each directory in the list of include directories
		for (const auto& dir : m_IncludeDirs)
		{
			std::wstring filePath = dir + L"\\" + std::wstring(pFileName, pFileName + strlen(pFileName)); // Convert pFileName (LPCSTR) to std::wstring

			// [AZB]: Try opening the file
			std::ifstream file(filePath);
			if (file.is_open())
			{
				// [AZB]: If we found it, carry on and read the file content into a string
				std::ifstream file(filePath, std::ios::binary);  // Open file in binary mode to avoid encoding issues
				if (file.is_open())
				{
					// [AZB]: Get the length of the file
					file.seekg(0, std::ios::end);
					size_t fileSize = file.tellg();
					file.seekg(0, std::ios::beg);

					// [AZB]: Allocate a buffer for the file contents
					char* fileContents = new char[fileSize];

					// [AZB]: Read the entire file into the buffer
					file.read(fileContents, fileSize);

					// [AZB]: Allocate a blob to store the file data
					ID3DBlob* pBlob = nullptr;
					HRESULT hr = D3DCreateBlob(static_cast<SIZE_T>(fileSize), &pBlob);
					if (FAILED(hr)) {
						delete[] fileContents;
						return hr;
					}

					// [AZB]: Copy the file data into the blob's buffer
					memcpy(pBlob->GetBufferPointer(), fileContents, fileSize);

					// [AZB]: Return the blob to the compiler
					*ppData = pBlob->GetBufferPointer();
					*pBytes = static_cast<UINT>(fileSize);

					// [AZB]: Clean up the file contents buffer
					delete[] fileContents;

					return S_OK;  // [AZB]: Successfully loaded the file so return success and exit function
				}
			}
			// [AZB]: Else, try the next directory in our list
		}
		// [AZB]: If we've gone through the entire list and not found it, we've failed
		return E_FAIL;
	}

	// [AZB]: Close method (no special cleanup needed in this case)
	STDMETHOD(Close)(THIS_ LPCVOID pData) override {
		// [AZB]: No specific cleanup is needed because we're not allocating memory dynamically here
		return S_OK;
	}


	// Helper function to read directories when we need them
	const std::vector<std::wstring>& GetDirectories() { return m_IncludeDirs; }
private:
	// [AZB]: Container of file paths that tell HLSL compiler where to look
	std::vector<std::wstring> m_IncludeDirs;
};






// Credit: Mark Featherstone
#if defined(DEBUG) | defined(_DEBUG)
#ifndef AETHER_HR_ASSERT													
inline void DXError(HRESULT hr, const char* pFileStr, int lineNum)
{
	LPSTR output;
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&output, 0, NULL);
	static char sErrBuffer[256];
	sprintf_s(sErrBuffer, 256, "FILE:%s \rLINE:%i \rERROR:%s", pFileStr, lineNum, output);
	MessageBox(0, sErrBuffer, "Error", 0);
	assert(false);
}

/*
Makes checking if functions worked neater
	e.x.
		HR(SomeD3DFunction());
If it fails the error checking code will get the error message, the
line number and file name and display it all. Saves us typing the
same thing over and over. The app will be stopped.
Just saves typing and stops silly bugs.
*/

// Credit: Mark Featherstone
#define AETHER_HR_ASSERT(x)												\
{															\
	HRESULT hr = (x);										\
	if(FAILED(hr))											\
		DXError( hr, __FILE__, __LINE__);					\
}

#endif
#else
#ifndef AETHER_HR_ASSERT
#define AETHER_HR_ASSERT(x) (x)
#endif
#endif 