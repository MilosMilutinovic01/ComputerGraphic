#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

// Uniform variable for the offset
uniform float uOffset;

void main()
{
    // Apply the offset to the X coordinate
    gl_Position = vec4(aPos.x + uOffset, aPos.y, 0.0, 1.0);
    TexCoord = aTexCoord;
}
