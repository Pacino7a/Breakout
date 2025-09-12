#version 330 core

out vec4 Color;

in vec2 TexCoords;
in vec4 ParticleColor;

uniform sampler2D spriteTexture;

void main()
{
    Color = (texture(spriteTexture, TexCoords) * ParticleColor);
}
