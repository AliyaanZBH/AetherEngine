#pragma once
//===============================================================================
// desc: High-level API for applications using Aether to render games with ease
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
#include "glm/glm.hpp"

#include "DrawCommand.h"
#include "Pipeline.h"
#include "Buffer.h"
#include "Drawing.h"
//===============================================================================

namespace Aether
{
	class IRendererBackend;
	class IWindow;
	class Camera;

	class AETHER_API Renderer
	{
	public:
		static void Initialise();
		static void Terminate();
		static void Resize(const uint16_t width, const uint16_t height);

		static void BeginFrame(const Camera& camera);
		static void Render();
		static void EndFrame();

		static void Draw(const eDrawGeoType drawType, const Transform& transform, MaterialInstance* mat);
		
		static void UploadMaterialInstance(MaterialInstance& instance);
		static void UpdateMaterialInstance(MaterialInstance& instance);


		// TMP: I feel these break the asbtraction a bit, but I'm putting them here just so we can get back to a stable build fast
		static void InitImGui();
		static void BeginImGuiRender();
		static void EndImGuiRender();


	private:

		//
		//	Set of lower-level functions that are hidden from the app to maintain the high-level abstraction, but must be implemented to facilitate the above.
		//

		static void CreateTriangleGeometry();
		static void CreateQuadGeometry();
		static void CreateBoxGeometry();


		template <typename T>
		static Buffer* CreateConstantBuffer(const eBufferType type, ConstantBufferView* cbv, uint8_t slot);

		// Create a default high-level description for a rendering pipeline, built by the backend
		static void CreateBackendPipeline();

		// Signal to backend API to execute the command queue
		static void Dispatch();

		// Clears command queue ready for the next frame
		static void Flush();

		// Currently running renderer backend
		static std::unique_ptr<IRendererBackend> s_RendererBackend;

		// Per-frame render lists, separated by material
		static std::vector<DrawCommand> s_SolidColourCommandQueue;
		static const std::string s_SolidColourPipeName;

		static std::vector<DrawCommand> s_BlinnPhongSolidColourCommandQueue;
		static const std::string s_BlinnPhongSolidColourPipeName;

		static std::vector<DrawCommand> s_TexturedCommandQueue;


		// Material objects
		// Single Material object that can be reused for multiple geometry
		static Aether::Material* s_FlatSolidColourMat;
		static Aether::Material* s_BlinnPhongLitSolidColourMat;

		//
		// Material data - shared across API
		//

		static std::vector<uint8_t> s_SolidColourMaterialDataCPU;
		static Buffer* s_SolidColourMaterialDataGPU;

		static std::vector<uint8_t> s_BPMaterialDataCPU;
		static Buffer* s_BPMaterialDataGPU;

		static uint32_t s_NextMatIdx;

		//
		// Geometry containers - shared between renderer backends
		//

		static Buffer* s_PerFrameBuffer;
		static ConstantBufferView s_PerFrameCBView;
		static Buffer* s_PerDrawBuffer;
		static ConstantBufferView s_PerDrawCBView;

		static GeometryBuffer* s_TriGeoBuffer;
		static GeometryBuffer* s_QuadGeoBuffer;
		static GeometryBuffer* s_BoxGeoBuffer;
	};
}