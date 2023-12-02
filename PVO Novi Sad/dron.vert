#version 330 core

layout(location = 0) in vec2 aPos;

uniform vec4 uColor;

uniform vec2 uPos;

void main()
{
    gl_Position = vec4(aPos + uPos, 0.0, 1.0);
}
