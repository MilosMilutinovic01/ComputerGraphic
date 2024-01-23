#version 330 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inCol;
uniform vec3 uPos;
out vec4 channelCol;

void main() //Glavna funkcija sejdera
{
	gl_Position = vec4(inPos.x * uPos.z + uPos.x, inPos.y * uPos.z + uPos.y, 0.0, 1.0);
	channelCol = inCol;
}