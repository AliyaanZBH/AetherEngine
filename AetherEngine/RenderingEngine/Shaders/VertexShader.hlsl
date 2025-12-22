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

//VS_OUTPUT main(uint id : SV_VertexID)
//{
//    VS_OUTPUT o;
//    o.pos = float4(0.5, 0.5, 0.5, 1); // force center
//    o.colour = float4(1, 0, 1, 1); // magenta
//    return o;
//}

//float4 main(uint id : SV_VertexID) : SV_POSITION
//{
//    float2 verts[3] =
//    {
//        float2(0.0f, 0.5f),
//        float2(0.5f, -0.5f),
//        float2(-0.5f, -0.5f)
//    };
//
//    return float4(verts[id], 0.0f, 1.0f);
//}