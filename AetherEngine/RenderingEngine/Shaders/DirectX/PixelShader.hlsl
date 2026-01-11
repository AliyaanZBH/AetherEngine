struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 colour : COLOR;
};


struct MaterialData
{
    float4 Colour;
};


StructuredBuffer<MaterialData> g_Materials : register(t0);

cbuffer PerDrawData : register(b1)
{
    float4x4 model;
    uint MaterialIndex;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	MaterialData mat = g_Materials[MaterialIndex];
    return mat.Colour;

}
