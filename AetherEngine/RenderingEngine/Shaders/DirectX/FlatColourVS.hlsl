struct VS_INPUT
{
    float4 pos : POSITION;
    float3 normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
};

cbuffer PerFrameData : register(b0)
{
    float4x4 u_ViewProjection;
    float4 u_CameraPos;
    float4 u_SunlightPos;
}

cbuffer PerDrawData : register(b1)
{
    float4x4 u_Model;
    float4x4 u_NormalMatrix;
    uint u_MaterialIndex;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    float4 world = mul(u_Model, input.pos);
    output.pos = mul(u_ViewProjection, world);
    
    return output;
}
