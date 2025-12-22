struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 colour : COLOR;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	// Gimme blue!
	//return float4(0.0f, 0.2f, 1.0f, 1.0f);
    return input.colour;

}

//float4 main() : SV_Target
//{
//    return float4(1, 0, 1, 1); // bright magenta
//}