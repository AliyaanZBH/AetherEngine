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
    float4x4 viewProjection;
}

cbuffer PerDrawData : register(b1)
{
    float4x4 model;
    float4   colour;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    float4 world = mul(model, input.pos);

    output.colour = colour;
    output.pos = mul(viewProjection, world);
    
    return output;
}
