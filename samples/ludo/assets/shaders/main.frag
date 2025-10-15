#version 450

// Inputs
layout(location = 0) in vec3 in_color;
layout(location = 1) in vec2 in_texture_uv;

// Outputs
layout(location = 0) out vec4 out_frag_color;

// Uniforms 
layout(set = 0, binding = 1) uniform sampler2D u_main_texture;

void main() {
    out_frag_color = texture(u_main_texture, in_texture_uv);
    // float depth_value = gl_FragCoord.z;
    // out_frag_color = vec4(vec3(depth_value), 1.0);
}
