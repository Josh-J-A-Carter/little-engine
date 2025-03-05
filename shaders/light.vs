#version 300 es

in vec3 in_position;

uniform mat4 u_model_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_proj_matrix;

out vec3 v_loc_pos;

void main() {
    v_loc_pos = in_position;
    gl_Position = u_proj_matrix * u_view_matrix * u_model_matrix * vec4(in_position, 1.0f);
}