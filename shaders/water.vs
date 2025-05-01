#version 300 es

in vec3 in_position;
in vec2 in_texcoord0;

uniform mat4 u_model_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_proj_matrix;

out vec2 v_texcoord0;
out vec4 v_clip_pos;
out vec3 v_world_pos;
// out vec3 v_normal;

out float v_w;

const float tiling_factor = 2.0f;

void main() {
    vec4 world_pos = u_model_matrix * vec4(in_position, 1.0f);
    v_clip_pos = u_proj_matrix * u_view_matrix * world_pos;
    gl_Position = v_clip_pos;
    v_w = gl_Position.w;

    v_texcoord0 = in_texcoord0 * tiling_factor;

    v_world_pos = world_pos.xyz;

    // v_normal = vec3(0, 1, 0);
}