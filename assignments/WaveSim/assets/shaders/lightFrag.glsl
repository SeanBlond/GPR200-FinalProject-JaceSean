#version 330 core
out vec4 FragColor;  
in vec3 normal;
in vec2 TexCoord;

uniform vec3 color;

void main()
{
    vec2 uv = TexCoord;

    FragColor = vec4(color, 1.0);
}