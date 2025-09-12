#version 330 core
layout (location = 0) in vec4 vertex; // <aPos, aTexCoords>

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform vec2 offset;
uniform vec4 color;

void main()
{
    float scale = 10.0f;
    ParticleColor = color;

    TexCoords = vertex.zw;
    gl_Position = projection * vec4((vertex.xy * scale) + offset, 0.0, 1.0); // we don't need to do transformation to the particles, so no model

}
