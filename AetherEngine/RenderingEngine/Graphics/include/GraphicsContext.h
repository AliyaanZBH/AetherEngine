#pragma once
//===============================================================================
// desc: A small helper class and enum to keep track of the current context for rendering
// auth: Aliyaan Zulfiqar
//===============================================================================

namespace Aether
{
	enum class eRenderAPI
	{
		kOpenGL,
		kDX11,
		kDX12
	};

	class GraphicsContext {
	public:
		static void SelectRenderAPI(eRenderAPI api) { s_CurrentRenderAPI = api; }
		static eRenderAPI GetRenderAPI() { return s_CurrentRenderAPI; }
	private:
		static inline eRenderAPI s_CurrentRenderAPI = eRenderAPI::kOpenGL;
	};
}
