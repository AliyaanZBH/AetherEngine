#version 430 core
layout (location = 0) out vec4 colour;

in vec4 v_Colour;

uniform vec4 u_DynamicColour;
uniform vec4 u_SolidColour;

void main()
{
    //colour = v_Colour + u_DynamicColour;
    colour = u_SolidColour + u_DynamicColour;
}
