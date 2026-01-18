struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 worldPos : POSITION;
    float3 normal : NORMAL;
};

struct MaterialData
{
    float4 diffuse;
    float3 specularR0;
    float roughness;
};

StructuredBuffer<MaterialData> g_Materials : register(t0);

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

static const float kPI = 3.14159;


float4 main(VS_OUTPUT input) : SV_TARGET
{
	MaterialData mat = g_Materials[u_MaterialIndex];
    float4 result = 1.0f;

    // Mostly taken from LearnOpenGL and a bit of Luna
    float shininess = 1.0 - mat.roughness;
    float3 N = normalize(input.normal);
    float3 worldPos = input.worldPos.xyz;
    
    // Temp hardcoded light vec
    // TODO: Put this in Cbuffer so that we can move the light around!
    float3 lightPos = float3(100.0, 100.0, 100.0);
    float3 lightDir = normalize(u_SunlightPos.xyz - worldPos);

    float3 viewDir = normalize(u_CameraPos.xyz - worldPos);

    // Half vec
    float3 H = normalize(lightDir + viewDir);
    float NdotH = max(dot(N, H), 0.0);

    // TODO: Read these from CBuffer

    // Ambient
    float ambientStrength = 0.2;
    float3 lightColour = float3(0.8, 0.8, 0.8);
    float3 ambient = ambientStrength * lightColour;

    // Lambert
    float NdotL = max(dot(N, lightDir), 0.0);
    float3 diffuse = lightColour * NdotL;
   
    // Calculate schlick fresnel - modified from Luna
    float cosIncidentAngle = saturate(dot(H,lightDir));
    float f0 = 1.0 - cosIncidentAngle;
    float3 r0 = mat.specularR0;    
    float3 fresnelFactor = r0 + (1.0 - r0) * pow(f0, 5.0);
    
    // Luna blinn-phong 
    //
    float m = shininess * 256.0;
    float roughnessFactor = (m + 8.0) * pow(NdotH, m) / (8.0 * kPI);
    float3 specular = roughnessFactor * fresnelFactor;

    // Credit: Frank Luna
    // Our spec formula goes outside [0,1] range, but we are 
    // doing LDR rendering.  So scale it down a bit.
    specular = specular / (specular + 1.0f);
    
    result = mat.diffuse * float4(ambient + diffuse + specular, 1.0);
    return result;
}
