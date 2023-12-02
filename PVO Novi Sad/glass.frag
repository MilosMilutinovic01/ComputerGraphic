#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

void main()
{
    FragColor = vec4(23.0/255.0, 81.0 / 255.0, 44.0 / 255.0, 0.7 ); // Transparent green color
}
