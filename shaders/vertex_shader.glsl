#version 410 core

in vec3 in_position;
in vec4 in_colour;

uniform mat4 u_model_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_pers_matrix;

out vec4 v_colour;

void main() {
    gl_Position = u_pers_matrix * u_view_matrix * u_model_matrix * vec4(in_position, 1.0f);
    v_colour = in_colour;
}