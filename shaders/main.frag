#version 450

struct UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
};

// Inputs
layout(location = 0) in vec3 in_color;

// Outputs
layout(location = 0) out vec4 out_frag_color;

void main() {
    out_frag_color = vec4(in_color, 1.0);
}
