#version 430 core

layout (std140, binding = 0) uniform PerFrameData
{
    mat4 u_ViewProjection;
    vec4 u_CameraPos;
};

layout (std140, binding = 1) uniform PerDrawData
{
    mat4 u_ModelMatrix;
    mat4 u_NormalMatrix;
    uint u_MaterialIndex;
};

layout (location = 0) in vec4 a_Pos;
layout (location = 1) in vec3 a_Norm;

out vec4 v_WorldPos;
out vec3 v_Normal;

void main()
{
    v_WorldPos = u_ModelMatrix * a_Pos;
    v_Normal = normalize(mat3(u_NormalMatrix) * a_Norm);

    gl_Position = u_ViewProjection * v_WorldPos;
}
