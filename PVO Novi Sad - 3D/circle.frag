#version 330 core


in vec4 channelCol;
uniform vec4 uCrcCol;
out vec4 outCol;

void main()
{
	outCol = uCrcCol;
}