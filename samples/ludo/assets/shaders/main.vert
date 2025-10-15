#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texture_uv;

layout(location = 0) out vec3 out_frag_color;
layout(location = 1) out vec2 out_texture_uv;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} u_mvp;

void main() {
    out_frag_color = vec3(0.0);
    out_texture_uv = in_texture_uv;
    gl_Position = u_mvp.proj * u_mvp.view * u_mvp.model * vec4(in_position, 1.0);
}
