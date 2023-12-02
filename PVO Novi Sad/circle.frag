#version 330 core

uniform vec3 customColor;
out vec4 outCol;

void main()
{
	outCol = vec4(customColor, 1.0);
}