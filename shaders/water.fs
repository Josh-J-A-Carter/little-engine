#version 300 es

precision highp float;

in vec4 v_clip_pos;

uniform sampler2D u_sampler_reflection;
uniform sampler2D u_sampler_refraction;

out vec4 out_color;

void main() {
    // out_color = vec4(0.2f, 0.1f, 1.0f, 1.0f);
    // vec4 reflect = texture(u_sampler_reflection, v_texcoord0);
    // vec4 refract = texture(u_sampler_refraction, v_texcoord0);

    vec2 ndc = v_clip_pos.xy / v_clip_pos.w;
    vec2 reflect_uv = 0.5f * vec2(ndc.x, -ndc.y) + 0.5f;
    vec2 refract_uv = 0.5f * vec2(ndc.x, ndc.y) + 0.5f;

    vec4 reflect = texture(u_sampler_reflection, reflect_uv);
    vec4 refract = texture(u_sampler_refraction, refract_uv);

    // out_color = reflect; 
    out_color = mix(reflect, refract, 0.5f);
}