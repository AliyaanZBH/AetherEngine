#version 430 core

layout (std140, binding = 0) uniform PerFrameData
{
    mat4 u_ViewProjection;
};

layout (std140, binding = 1) uniform PerDrawData
{
    mat4 u_ModelMatrix;
    vec4 u_SolidColour;
};

layout (location = 0) in vec4 a_Pos;
layout (location = 1) in vec4 a_Col;

out vec4 v_Colour;

void main()
{
    gl_Position = u_ViewProjection * u_ModelMatrix * a_Pos;
	v_Colour = u_SolidColour;
}
