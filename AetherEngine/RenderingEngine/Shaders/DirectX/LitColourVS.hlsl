struct VS_INPUT
{
    float4 pos : POSITION;
    float3 normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 worldPos : POSITION;
    float3 normal : NORMAL;
};

cbuffer PerFrameData : register(b0)
{
    float4x4 u_ViewProjection;
    float4 u_CameraPos;
}

cbuffer PerDrawData : register(b1)
{
    float4x4 u_ModelMatrix;
    float4x4 u_NormalMatrix;
    uint u_MaterialIndex;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    float4 world = mul(u_ModelMatrix, input.pos);
    output.worldPos = world;
    output.normal = normalize(mul(u_NormalMatrix, input.normal));
    output.pos = mul(u_ViewProjection, world);
    
    return output;
}
