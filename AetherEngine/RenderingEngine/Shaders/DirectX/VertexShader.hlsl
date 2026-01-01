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

cbuffer PerDrawData : register(b0)
{
    float4x4 model;
    float4   colour;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    //output.pos = input.pos;
    //output.colour = input.colour;
    output.colour = colour;
    output.pos = mul(model, input.pos);
    return output;
}
