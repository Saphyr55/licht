#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texture_uv;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_texture_uv;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 view_proj;
    vec3 eye_position;
} u_ubo;

layout(push_constant) uniform RenderItem {
    mat4 model;
} u_item;

void main() {
    vec4 world_position = u_item.model * vec4(in_position, 1.0);
    vec4 world_normal = u_item.model * vec4(in_normal, 1.0);

    out_position = world_position.xyz;
    out_normal = world_normal.xyz;
    out_texture_uv = in_texture_uv;
    
    gl_Position = u_ubo.view_proj * world_position;
}
