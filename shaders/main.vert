#version 450

// Inputs
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

// Outputs
layout(location = 0) out vec3 out_frag_color;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} u_mvp;

void main() {
    out_frag_color = in_color;
    gl_Position = u_mvp.proj * u_mvp.view * u_mvp.model * vec4(in_position, 1.0);
}
