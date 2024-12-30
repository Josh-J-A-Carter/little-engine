#version 410 core

in vec3 in_position;
in vec4 in_colour;

out vec4 v_colour;

void main() {
    gl_Position = vec4(in_position.x, in_position.y, in_position.z, 1.0f);
    v_colour = in_colour;
}