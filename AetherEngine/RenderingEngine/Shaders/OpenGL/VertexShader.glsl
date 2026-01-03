#version 430 core
layout (location = 0) in vec4 a_Pos;
layout (location = 1) in vec4 a_Col;

uniform mat4 u_Transform;

out vec4 v_Colour;

void main()
{
    gl_Position = u_Transform * a_Pos;
	v_Colour = a_Col;
}