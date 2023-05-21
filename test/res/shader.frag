#version 450

layout (binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
} ubo;

layout (binding = 1) uniform sampler2D main_texture;
layout (binding = 2) uniform samplerCube skybox;

layout (location = 0) in vec3 frag_colour;
layout (location = 1) in vec2 frag_uv;
layout (location = 2) in vec3 frag_position;

layout (location = 0) out vec4 o_colour;

void main()
{
	vec3 dir = normalize(frag_position);
	vec4 reflected_colour = texture(skybox, dir) * vec4(frag_colour, 1.0);
	vec4 texture_colour = texture(main_texture, frag_uv) * vec4(frag_colour, 1.0);
	o_colour = mix(reflected_colour, vec4(0.5), 0.2);
	o_colour = mix(o_colour, texture_colour, 0.5);
	o_colour = vec4(frag_uv, 0.0, 1.0);
}
