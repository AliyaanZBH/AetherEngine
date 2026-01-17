#version 430 core

layout (std140, binding = 0) uniform PerFrameData
{
    mat4 u_ViewProjection;
};

layout (std140, binding = 1) uniform PerDrawData
{
    mat4 u_ModelMatrix;
    uint u_MaterialIndex;
};

layout (location = 0) in vec4 a_Pos;
layout (location = 1) in vec3 a_Norm;

void main()
{
    vec4 world = u_ModelMatrix * a_Pos;
    gl_Position = u_ViewProjection * world;
}
