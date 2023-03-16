#version 450

layout (location = 0) in vec3 frag_colour;
layout (location = 1) in vec2 frag_uv;

layout (location = 0) out vec4 o_colour;

layout (binding = 1) uniform sampler2D texture0;

void main()
{
    o_colour = texture(texture0, frag_uv) * vec4(frag_colour, 1.0);
}
