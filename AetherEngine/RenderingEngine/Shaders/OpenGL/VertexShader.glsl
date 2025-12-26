#version 430 core
layout (location = 0) in vec4 a_Pos;
layout (location = 1) in vec4 a_Col;

out vec4 v_Colour;

void main()
{
    gl_Position = a_Pos;
	v_Colour = a_Col;
}