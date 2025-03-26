#version 300 es

in vec3 in_position;
in vec2 in_texcoord0;
in vec3 in_normal;

uniform mat4 u_model_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_proj_matrix;

uniform mat4 u_shadow_matrix;

out vec3 v_world_pos;
out vec2 v_texcoord0;
out vec3 v_normal;
out vec4 v_lightspace_pos;

void main() {
    vec4 world_pos = u_model_matrix * vec4(in_position, 1.0f);
    gl_Position = u_proj_matrix * u_view_matrix * world_pos;
    v_world_pos = world_pos.xyz;
    v_texcoord0 = in_texcoord0;
    v_normal = (u_model_matrix * vec4(in_normal, 0)).rgb;

    v_lightspace_pos = u_shadow_matrix * u_model_matrix * vec4(in_position, 1.0f);
}