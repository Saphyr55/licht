#version 450

struct PunctualLight {
    vec3 position;
    vec3 color;
};

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texture_uv;
layout(location = 3) in vec3 in_tangent;
layout(location = 4) in vec3 in_bitangent;

layout(location = 0) out vec4 out_frag_color;

layout(std140, set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 view_proj;
    vec3 eye_position;
} u_ubo;

layout(std140, set = 0, binding = 1) uniform ULights {
    PunctualLight punctual_light;
} u_lights;

layout(set = 1, binding = 0) uniform sampler2D u_samplers2D[];

float inverse_square_curve_windowing(float light_distance, float max_distance) {
    float value = 1.0 - pow(light_distance / max_distance, 4);
    float clamped_value = clamp(value, 0.0, value);
    return clamped_value * clamped_value;
}

float inverse_square_light_attenuation(float light_distance, float fixed_distance, float epsilon) {
    return (fixed_distance * fixed_distance) / ((light_distance * light_distance) + epsilon);
}

void main() {
    vec4 diffuse = texture(u_samplers2D[0], in_texture_uv);
    vec3 normal_map = texture(u_samplers2D[1], in_texture_uv).rgb;

    vec3 normal = normalize(in_normal);
    vec3 tangent = normalize(in_tangent);
    vec3 bitangent = normalize(in_bitangent);

    mat3 TBN = mat3(tangent, bitangent, normal);

    if (length(normal_map) > 0.0) {
        normal = normal_map * 2.0 - 1.0;
        normal = normalize(TBN * normal);
    }

    vec3 view_direction = normalize(u_ubo.eye_position - in_position);
    if (dot(view_direction, normal) < 0.0) {
        normal = -normal;
    }

    PunctualLight punctual_light = u_lights.punctual_light;

    vec3 light_direction = punctual_light.position - in_position;
    float light_distance = length(light_direction);
    light_direction = normalize(light_direction);

    vec3 halfway_direction = normalize(light_direction + view_direction);

    float NdotL = max(dot(normal, light_direction), 0.0);
    float NdotH = max(dot(normal, halfway_direction), 0.0);

    float shininess = 64.0;
    float specular = NdotL > 0.0 ? pow(NdotH, shininess) : 0.0;
    vec3 result = 0.01 * diffuse.rgb +
                  NdotL * diffuse.rgb +
                  specular * punctual_light.color;

    float epsilon = 1.0e-5;
    result *= inverse_square_light_attenuation(light_distance, 1.0, epsilon);

    out_frag_color = vec4(result, diffuse.a);
}
