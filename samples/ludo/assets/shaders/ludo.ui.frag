#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec2 in_texture_uv;

layout(location = 0) out vec4 out_frag_color;

layout(std140, set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 view_proj;
} u_ubo;

layout(push_constant) uniform DrawItem {
    mat4 model;
} u_item;

void main() {
    out_frag_color = vec4(0.0);
}