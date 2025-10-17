#version 450

#define MAX_PUNCTUAL_LIGHT 16

struct PunctualLight {
    vec3 position;
    vec3 color;
};

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texture_uv;

layout(location = 0) out vec4 out_frag_color;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 view_proj;
    vec3 eye_position;
} u_ubo;

layout(set = 0, binding = 1) uniform ULight {
    PunctualLight punctual_light;
} u_light;

layout(set = 1, binding = 0) uniform sampler2D u_diffuse_map;
layout(set = 1, binding = 1) uniform sampler2D u_normal_map;

// Source: Real-Time Rendering Fourth Edition, Chapter 5.3
vec3 lit(vec3 light_direction, vec3 normal, vec3 view_direction, vec3 cwarm, vec3 highlight) {
    vec3 direction_reflect = reflect(-light_direction, normal);
    float s = clamp(100.0 * dot(direction_reflect, view_direction) - 97.0, 0.0, 1.0);
    return mix(cwarm, highlight, s); // Interpolation
}

vec3 unlit(vec3 normal, vec3 view_direction, vec3 diffuse_cool) {
    return diffuse_cool / 2;
}

void main() {
    vec4 diffuse = texture(u_diffuse_map, in_texture_uv);

    vec3 normal; 
    normal = normalize(in_normal);
    // normal = normalize(texture(u_normal_map, in_texture_uv).xyz);
    
    vec3 view_direction = normalize(u_ubo.eye_position - in_position);

    vec3 highlight = vec3(2.0, 2.0, 2.0);
    vec3 diffuse_cool = vec3(0.0, 0.0, 0.50) + 0.25 * diffuse.rgb;
    vec3 diffuse_warm = vec3(0.33, 0.33, 0.0) + 0.25 * diffuse.rgb;

    vec3 light_direction = normalize(u_light.punctual_light.position - in_position);
    
    float geometric_term = clamp(dot(normal, light_direction), 0.0, 1.0);
    
    vec3 fcolor = unlit(normal, view_direction, diffuse_cool);
    fcolor += geometric_term * u_light.punctual_light.color * lit(light_direction, normal, view_direction, diffuse_warm, highlight);

    out_frag_color = vec4(fcolor, 1.0); 
}
