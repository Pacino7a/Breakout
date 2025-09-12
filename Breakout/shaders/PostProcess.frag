#version 330 core

out vec4 Color;

in vec2 TexCoords;

uniform sampler2D renderResult;

uniform vec2  offsets[9];
uniform int   edge_kernel[9];
uniform float blur_kernel[9];

uniform bool chaos;
uniform bool confuse;
uniform bool shake;

void main()
{
    Color = vec4(0.0f);
    vec3 sample[9];
    if (chaos || shake)
    {
        for(int i = 0; i < 9; ++i)
        {
            sample[i] = texture(renderResult, TexCoords + offsets[i]).rgb;
        }
    }

    if (chaos)
    {
        for(int i = 0; i < 9; ++i)
        {
            Color += vec4(sample[i] * edge_kernel[i], 0.0f);
        }
        Color.a = 1.0f;
    }
    else if (confuse)
    {
        Color = vec4(1.0f - texture(renderResult, TexCoords).rgb, 1.0f);
    }
    else if (shake)
    {
        for(int i = 0; i < 9; ++i)
        {
            Color += vec4(sample[i] * blur_kernel[i], 0.0f);
        }
        Color.a = 1.0f;
    }
    else
    {
        Color = texture(renderResult, TexCoords);
    }
}
