#version 450

// Inputs
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

// Outputs
layout(location = 0) out vec3 out_frag_color;

void main() {
    out_frag_color = in_color;
    gl_Position = vec4(in_position, 1.0);
}
