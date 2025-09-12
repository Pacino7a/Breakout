#version 330 core
out vec4 Color;

in vec2 TexCoords;

uniform sampler2D fontBitMap;
uniform vec3 textColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(fontBitMap, TexCoords).r); // use alpha channel to control the visability of Text
    Color = vec4(textColor, 1.0) * sampled; // if we don't sample, color will become color blocks.
}
