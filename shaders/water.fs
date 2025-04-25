#version 300 es

precision highp float;

in vec4 v_clip_pos;
in vec2 v_texcoord0;

uniform sampler2D u_sampler_reflection;
uniform sampler2D u_sampler_refraction;
uniform sampler2D u_sampler_dudv;

uniform float u_time;

out vec4 out_color;

const float distortion_strength = 0.01f;
const float time_factor = 0.02f;

void main() {
    // out_color = vec4(0.2f, 0.1f, 1.0f, 1.0f);
    // vec4 reflect = texture(u_sampler_reflection, v_texcoord0);
    // vec4 refract = texture(u_sampler_refraction, v_texcoord0);

    vec2 ndc = v_clip_pos.xy / v_clip_pos.w;
    vec2 reflect_uv = 0.5f * vec2(ndc.x, -ndc.y) + 0.5f;
    vec2 refract_uv = 0.5f * vec2(ndc.x, ndc.y) + 0.5f;

    float t = fract(u_time * time_factor);

    vec4 dudv1 = texture(u_sampler_dudv, vec2(v_texcoord0.x + t, v_texcoord0.y));
    vec4 dudv2 = texture(u_sampler_dudv, vec2(-v_texcoord0.x + t, v_texcoord0.y + t));
    vec2 d1 = vec2(2.0f * dudv1.r - 1.0f, 2.0f * dudv1.g - 1.0f) * distortion_strength;
    vec2 d2 = vec2(2.0f * dudv2.r - 1.0f, 2.0f * dudv2.g - 1.0f) * distortion_strength;
    reflect_uv += d1 + d2;
    refract_uv += d1 + d2;

    reflect_uv = vec2(clamp(reflect_uv.x, 0.001f, 0.999f), clamp(reflect_uv.y, 0.001f, 0.999f));
    refract_uv = vec2(clamp(refract_uv.x, 0.001f, 0.999f), clamp(refract_uv.y, 0.001f, 0.999f));

    vec4 reflect = texture(u_sampler_reflection, reflect_uv);
    vec4 refract = texture(u_sampler_refraction, refract_uv);

    // out_color = mix(reflect, refract, 0.5f);
    out_color = mix(mix(reflect, refract, 0.5f), vec4(0.6f, 0.6f, 1.0f, 1.0f), 0.2f);
}