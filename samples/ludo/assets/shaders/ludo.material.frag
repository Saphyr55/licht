#version 450

#define MAX_PUNCTUAL_LIGHT 16

struct PunctualLight {
    vec3 position;
    vec3 color;
};

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texture_uv;
layout(location = 3) in vec4 in_tangent;

layout(location = 0) out vec4 out_frag_color;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 view_proj;
    vec3 eye_position;
} u_ubo;

layout(set = 0, binding = 1) uniform ULights {
    PunctualLight punctual_light;
} u_lights;

layout(set = 1, binding = 0) uniform sampler2D u_diffuse_map;
layout(set = 1, binding = 1) uniform sampler2D u_normal_map;

void main() {
    vec4 diffuse = texture(u_diffuse_map, in_texture_uv);
    vec3 normal_map = texture(u_normal_map, in_texture_uv).xyz * 2.0 - 1.0;

    vec3 normal = normalize(in_normal);
	vec3 tangent = normalize(in_tangent.xyz);
    tangent = normalize(tangent - dot(tangent, normal) * normal);
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);
    normal = normalize(TBN * normal);

    vec3 tangent_position = TBN * in_position;

    PunctualLight punctual_light = u_lights.punctual_light;

    vec3 light_incident = normalize(TBN * punctual_light.position - tangent_position);
    vec3 view_direction = normalize(TBN * u_ubo.eye_position - tangent_position);
    normal = dot(view_direction, normal) > 0.0 ? normal : -normal;

    float geometric_term = max(dot(normal, light_incident), 0.0);
    vec3 result = geometric_term * diffuse.rgb;

    vec3 reflect_direction = reflect(-light_incident, normal);
    // vec3 halfway_direction = normalize(light_incident + view_direction);
    const float shininess = 16.0;
    float specular = dot(normal, reflect_direction);
    specular = clamp(specular, 0.0, 1.0);
    specular = pow(specular, shininess);

    result += specular;
    result *= punctual_light.color;

    out_frag_color = vec4(result, 1.0); 
}
