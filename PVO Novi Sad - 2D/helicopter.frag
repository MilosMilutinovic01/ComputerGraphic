#version 330 core

out vec4 outCol;
uniform float blink;

void main()
{
	outCol = vec4(254.0/255.0 + blink, 23.0/255.0 + blink, 28.0/255.0 + blink, 1.0 );
}