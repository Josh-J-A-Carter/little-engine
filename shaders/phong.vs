#version 300 es

in vec3 in_position;
in vec2 in_texcoord0;
in vec3 in_normal;

uniform mat4 u_model_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_proj_matrix;

uniform mat4 u_shadow_matrix;

uniform vec4 u_clip_plane;
uniform float u_clip_enabled;

out vec3 v_world_pos;
out vec2 v_texcoord0;
out vec3 v_normal;
out vec4 v_lightspace_pos;

out float v_w;

out float v_clip;

void main() {
    vec4 world_pos = u_model_matrix * vec4(in_position, 1.0f);
    gl_Position = u_proj_matrix * u_view_matrix * world_pos;
    v_w = gl_Position.w;
    v_world_pos = world_pos.xyz;
    v_texcoord0 = in_texcoord0;
    v_normal = (u_model_matrix * vec4(in_normal, 0.0f)).rgb;

    v_lightspace_pos = u_shadow_matrix * u_model_matrix * vec4(in_position, 1.0f);

    v_clip = dot(vec4(v_world_pos, 1.0f), u_clip_plane) * u_clip_enabled;
}