#version 410 core

in vec2 v_texcoord0;

uniform sampler2D u_sampler;

out vec4 out_colour;

void main() {
    out_colour = texture2D(u_sampler, v_texcoord0);
}