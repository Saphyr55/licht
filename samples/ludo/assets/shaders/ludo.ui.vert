#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec2 in_texture_uv;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec4 out_color;
layout(location = 2) out vec2 out_texture_uv;

layout(std140, set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 view_proj;
    vec3 unused;
} u_ubo;

layout(push_constant) uniform DrawItem {
    mat4 model;
} u_item;

void main() {
    vec4 position = u_item.model * vec4(in_position, 1.0);

    out_position = position.xyz;
    out_color = in_color;
    out_texture_uv = in_texture_uv;

    gl_Position = u_ubo.view_proj * position;
}