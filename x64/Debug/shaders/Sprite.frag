#version 330 core

out vec4 Color;

in vec2 TexCoords;

uniform sampler2D spriteTexture;
uniform vec3 spriteColor;

void main()
{
    vec4 sampled = texture(spriteTexture, TexCoords);
    vec4 mixColor = vec4(spriteColor, 1.0) * sampled;
    Color = mixColor;

    // vec3 sampled = texture(spriteTexture, TexCoords).rgb;
    // vec3 mixColor = spriteColor * sampled;
    // Color = vec4(mixColor, 1.0);
}
