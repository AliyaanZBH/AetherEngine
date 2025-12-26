#version 430 core
layout (location = 0) out vec4 colour;

in vec4 v_Colour;

uniform vec4 dynamicColour;

void main()
{
    colour = v_Colour + dynamicColour;
}
