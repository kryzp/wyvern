#version 450

layout (location = 0) in vec2 i_position;
layout (location = 1) in vec3 i_colour;

layout (location = 0) out vec3 frag_colour;

void main()
{
    gl_Position = vec4(i_position, 0.0, 1.0);
    frag_colour = i_colour;
}
