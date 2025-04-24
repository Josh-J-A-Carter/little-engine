#version 300 es

precision highp float;

in vec3 v_world_pos;
in vec2 v_texcoord0;
in vec3 v_normal;

uniform sampler2D u_sampler_reflection;

out vec4 out_color;

void main() {
    // out_color = vec4(0.2f, 0.1f, 1.0f, 1.0f);
    out_color = texture(u_sampler_reflection, vec2(v_texcoord0.x, v_texcoord0.y));
}