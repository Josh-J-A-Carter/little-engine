#version 300 es

precision highp float;

uniform float u_time;

in vec3 v_loc_pos;

out vec4 out_color;

void main() {
    vec3 dir_to_centre = vec3(0, 0, 0) - v_loc_pos;
    float dist = length(dir_to_centre);
    out_color = vec4(1.0f, 1.0f, 0.0f, 0.75f) * (0.8 - dist);
}