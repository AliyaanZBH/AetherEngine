//===============================================================================
// desc: A small utility class to handle pipeline creation across rendering APIs (GLSL & HLSL)
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Shader.h"
#include "GraphicsContext.h"
//===============================================================================

namespace Aether
{
    using PipelineHandle = uint32_t;

	struct PipelineDesc
	{
        ShaderHandle m_VertexShader;
        ShaderHandle m_PixelShader;
        VertexLayout m_Layout;

        //RasterState Raster;
        //DepthStencilState Depth;
        //BlendState Blend;
	};
}