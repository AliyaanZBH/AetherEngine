#pragma once
//===============================================================================
// desc: Shader class implementation for OpenGL
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "ShaderOpenGL.h"
#include "AetherUtils.h"
//===============================================================================

namespace Aether
{
	ShaderOpenGL::ShaderOpenGL(const std::string& shaderFilename, eShaderStage stage)
	{
		//
		//	Taken more or less verbatim from Khronos doc: https://wikis.khronos.org/opengl/Shader_Compilation
		//

		// Modified to only load a single shader from file


		std::string shaderDir = "Shaders\\OpenGL\\";  // [AZB]: Main GLSL shader directory.
		std::string fullShaderPath = shaderDir + shaderFilename + ".glsl";
		std::string shaderSrc = "";

		// Prep the filepath
		std::ifstream shaderFile;
		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		// Read our shader file and store data into the appropriate buffers
		// TODO: Doing it old-school with a try catch for now (based off of https://learnopengl.com/Getting-started/Shaders), will modernise later
		try
		{
			// Open the file if it exists
			shaderFile.open(fullShaderPath.c_str());
			std::stringstream shaderStream;

			// Read file's buffer contents into streams
			shaderStream << shaderFile.rdbuf();

			// Close file handlers
			shaderFile.close();
			
			// convert stream into string
			shaderSrc = shaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			AETHER_ASSERT(AETHER_FAIL, "GLSL Shader file could not be read");
		}

		// Create an empty shader handle - use the passed in type (e.g. kVertex which get's converted to GL_VERTEX_SHADER)
		GLuint shaderHandle = glCreateShader(ShaderStageToGLSLCompilerEnum(stage));

		// Send the shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = (const GLchar*)shaderSrc.c_str();
		glShaderSource(shaderHandle, 1, &source, 0);

		// Compile the shader
		glCompileShader(shaderHandle);

		GLint isCompiled = 0;
		glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shaderHandle, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(shaderHandle);

			// Use the infoLog as you see fit.
			AETHER_ASSERT(AETHER_FAIL, " GLS Shader failed to compile: {0}", infoLog.data());

			// In this simple program, we'll just leave
			return;
		}

		// Shader successfully compiled.
		// Now time to link into a program.
		// Get a program object.
		m_GLProgram = glCreateProgram();

		// Attach shader to our program
		glAttachShader(m_GLProgram, shaderHandle);

		// Link our program
		glLinkProgram(m_GLProgram);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(m_GLProgram, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_GLProgram, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_GLProgram, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(m_GLProgram);
			// Don't leak shaders either.
			glDeleteShader(shaderHandle);

			AETHER_ASSERT(AETHER_FAIL, "Shaders failed to link: {0}", infoLog.data());
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(m_GLProgram, shaderHandle);
	}

	GLenum ShaderOpenGL::ShaderStageToGLSLCompilerEnum(eShaderStage stage)
	{
		switch (stage)
		{
			case eShaderStage::kVertex: return GL_VERTEX_SHADER;
			case eShaderStage::kPixel: return GL_FRAGMENT_SHADER;
		}

		AETHER_ASSERT(AETHER_FAIL, "Unknown shader stage for OpenGL");
		return GL_SHADER;
	}

	void ShaderOpenGL::Bind()
	{
		glUseProgram(m_GLProgram);
	}
	void ShaderOpenGL::Unbind()
	{
		glUseProgram(0);
	}
}