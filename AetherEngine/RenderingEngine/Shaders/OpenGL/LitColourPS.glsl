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
    vec4 u_SunlightPos;
};

layout (std140, binding = 1) uniform PerDrawData
{
    mat4 u_ModelMatrix;
    mat4 u_NormalMatrix;
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
    vec3 lightDir = normalize(u_SunlightPos.xyz - v_WorldPos.xyz);
    vec3 viewDir = normalize(u_CameraPos.xyz - v_WorldPos.xyz);

    // Half vec
    vec3 H = normalize(lightDir + viewDir);
    float NdotH = max(dot(N, H), 0.0);

    // TODO: Read these from CBuffer

    // Ambient
    float ambientStrength = 0.2;
    vec3 lightColour = vec3(0.9, 0.9, 0.9);
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

    colour = mat.diffuse * vec4(ambient + diffuse + specular, 1.0);
}

