#version 450

// Inputs
layout(location = 0) in vec3 in_color;
layout(location = 1) in vec2 in_texture_uv;

// Outputs
layout(location = 0) out vec4 out_frag_color;

// Uniforms 
layout(binding = 1) uniform sampler2D orange_texture;

void main() {
    float depth_value = gl_FragCoord.z;

    out_frag_color = texture(orange_texture, in_texture_uv);
}
