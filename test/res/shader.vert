#version 450

layout (binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec2 i_uv;
layout (location = 2) in vec3 i_colour;

layout (location = 0) out vec3 frag_colour;
layout (location = 1) out vec2 frag_uv;

void main()
{
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(i_position, 1.0);
    frag_colour = i_colour;
    frag_uv = i_uv;
}
