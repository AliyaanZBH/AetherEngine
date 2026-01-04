struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 colour : COLOR;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	// Return vertex colour
    return input.colour;

}
