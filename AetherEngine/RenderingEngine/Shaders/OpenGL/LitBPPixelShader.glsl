#version 430 core

struct MaterialData
{
    vec4 diffuse;
    vec3 specularR0;
    float roughness;
};

layout(std430, binding = 0) buffer MaterialBuffer
{
    MaterialData materials[];
};

layout (std140, binding = 0) uniform PerFrameData
{
    mat4 u_ViewProjection;
    vec4 u_CameraPos;
};

layout (std140, binding = 1) uniform PerDrawData
{
    mat4 u_ModelMatrix;
    uint u_MaterialIndex;
};

layout (location = 0) out vec4 colour;

in vec4 v_WorldPos;
in vec3 v_Normal;

const float kPI = 3.14159;

void main()
{
    MaterialData mat = materials[u_MaterialIndex];

    // Mostly taken from LearnOpenGL and a bit of Luna
    float shininess = 1.0 - mat.roughness;
    vec3 N = normalize(v_Normal);
    
    // Temp hardcoded light vec
    // TODO: Put this in Cbuffer so that we can move the light around!
    vec3 lightPos = vec3(100.0, 100.0, 100.0);
    vec3 lightDir = normalize(lightPos - v_WorldPos.xyz);

    vec3 viewDir = normalize(u_CameraPos.xyz - v_WorldPos.xyz);

    // Half vec
    vec3 H = normalize(lightDir + viewDir);
    float NdotH = max(dot(N, H), 0.0);

    // TODO: Read these from CBuffer

    // Ambient
    float ambientStrength = 0.2;
    vec3 lightColour = vec3(0.8, 0.8, 0.8);
    vec3 ambient = ambientStrength * lightColour;

    // Lambert
    float NdotL = max(dot(N, lightDir), 0.0);
    vec3 diffuse = lightColour * NdotL;
   
    // Calculate schlick fresnel - modified from Luna
    float cosIncidentAngle = clamp(dot(H,lightDir), 0.0, 1.0);
    float f0 = 1.0 - cosIncidentAngle;
    vec3 r0 = mat.specularR0;    
    vec3 fresnelFactor = r0 + (1.0 - r0) * pow(f0, 5.0);
    
    // Luna blinn-phong 
    //
    float m = shininess * 256.0;
    float roughnessFactor = (m + 8.0) * pow(NdotH, m) / (8.0 * kPI);
    vec3 specular = roughnessFactor * fresnelFactor;

    // Credit: Frank Luna
    // Our spec formula goes outside [0,1] range, but we are 
    // doing LDR rendering.  So scale it down a bit.
    specular = specular / (specular + 1.0f);

    // TMP: Slightly more advanced method using Disney roughness that I couldn't quite get working right
    
    // Convert roughness to alpha, adopting Disney’s reparameterization of a = Roughness^2
    //float alpha = mat.roughness * mat.roughness; 
    //float a2 = alpha * alpha; 

    //float power = (2.0 / a2) - 2.0;

    //float roughnessFactor = (1 / (PI * a2)) * pow(max(NdotH, 0.0f), power);

    // Calculate final specular 
    //vec3 specular = mat.specular.rgb * pow(NdotH, mat.roughness);
    //vec3 specular = fresnelFactor * roughnessFactor;

    colour = mat.diffuse * vec4(ambient + diffuse + specular, 1.0);
}

