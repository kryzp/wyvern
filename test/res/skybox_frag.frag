#version 450

layout (location = 0) in vec3 frag_colour;
layout (location = 1) in vec2 frag_uv;
layout (location = 2) in vec3 frag_position;

layout (location = 0) out vec4 o_colour;

layout (binding = 1) uniform samplerCube cube_sampler;

void main()
{
	vec3 dir = normalize(frag_position);
	o_colour = texture(cube_sampler, dir) * vec4(frag_colour, 1.0);
}
