#version 450

layout (location = 0) in vec3 frag_colour;
layout (location = 1) in vec2 frag_uv;
layout (location = 2) in vec3 frag_position;

layout (location = 0) out vec4 o_colour;

layout (binding = 1) uniform sampler2D texture0;
layout (binding = 2) uniform samplerCube skybox;

void main()
{
	vec3 dir = reflect(normalize(frag_position), vec3(0.0, 0.0, -1.0));
	vec4 reflected_colour = texture(skybox, dir) * vec4(frag_colour, 1.0);
	vec4 texture_colour = texture(texture0, frag_uv) * vec4(frag_colour, 1.0);
	o_colour = mix(reflected_colour, vec4(0.5), 0.2);
	o_colour = mix(o_colour, texture_colour, 0.0);
}
