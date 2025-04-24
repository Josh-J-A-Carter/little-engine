#version 300 es

in vec3 in_position;
in vec2 in_texcoord0;
in vec3 in_normal;

uniform mat4 u_model_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_proj_matrix;

out vec4 v_clip_pos;

void main() {
    v_clip_pos =  u_proj_matrix * u_view_matrix * u_model_matrix * vec4(in_position, 1.0f);
    gl_Position = v_clip_pos;
}