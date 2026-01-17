struct VS_INPUT
{
    float4 pos : POSITION;
    float4 colour : COLOR;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 colour : COLOR;
};

cbuffer PerFrameData : register(b0)
{
    float4x4 u_ViewProjection;
}

cbuffer PerDrawData : register(b1)
{
    float4x4 u_Model;
    uint u_MaterialIndex;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    float4 world = mul(u_Model, input.pos);

    output.colour = input.colour;
    output.pos = mul(u_ViewProjection, world);
    
    return output;
}
