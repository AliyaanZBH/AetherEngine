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

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = input.pos;
    output.colour = input.colour;
    return output;
}
