#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texture_uv;
layout(location = 3) in vec4 in_tangent;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_texture_uv;
layout(location = 3) out vec3 out_tangent;
layout(location = 4) out vec3 out_bitangent;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 view_proj;
    vec3 eye_position;
} u_ubo;

layout(push_constant) uniform UDrawItem {
    mat4 model;
} u_item;

void main() {
    vec4 world_position = u_item.model * vec4(in_position, 1.0);

    mat3 m3_model = mat3(u_item.model);
    mat3 normal_matrix = mat3(u_item.model); 

    vec3 normal = normalize(normal_matrix * in_normal);
    vec3 tangent = normalize(normal_matrix * in_tangent.xyz);
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 bitangent = cross(normal, tangent);

    out_position = world_position.xyz;
    out_bitangent = bitangent;
    out_normal = normal;
    out_tangent = tangent;
    out_texture_uv = in_texture_uv;

    gl_Position = u_ubo.view_proj * world_position;
}
