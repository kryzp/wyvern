#version 450

layout (location = 0) out vec3 frag_colour;

vec2 g_positions[3] = vec2[](
    vec2( 0.75, -0.75),
    vec2( 0.75,  0.75),
    vec2(-0.75,  0.75)
);

vec3 g_colours[3] = vec3[](
    vec3(1.00, 0.15, 0.00),
    vec3(0.15, 1.00, 0.15),
    vec3(0.00, 0.15, 1.00)
);

void main()
{
    gl_Position = vec4(g_positions[gl_VertexIndex], 0.0, 1.0);
    frag_colour = g_colours[gl_VertexIndex];
}
