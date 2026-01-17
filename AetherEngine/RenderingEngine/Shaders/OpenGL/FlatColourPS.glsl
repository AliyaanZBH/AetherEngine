#version 430 core

struct MaterialData
{
    vec4 colour;
};

layout(std430, binding = 0) buffer MaterialBuffer
{
    MaterialData materials[];
};

layout (std140, binding = 1) uniform PerDrawData
{
    mat4 u_ModelMatrix;
    uint u_MaterialIndex;
};

layout (location = 0) out vec4 colour;

in vec4 v_Colour;

void main()
{
    MaterialData mat = materials[u_MaterialIndex];
    colour = mat.colour;
}
