#version 300 es

precision highp float;

in vec4 v_clip_pos;
in vec2 v_texcoord0;
in vec3 v_world_pos;
// in vec3 v_normal;

const int MAX_DIR_LIGHTS = 10;

struct light {
    vec3 color;
    float ambient_intensity;
    float diffuse_intensity;
    float specular_intensity;
};

struct dir_light {
    light base;
    vec3 direction;
};

uniform int u_num_dir_lights;
uniform dir_light u_dir_lights[MAX_DIR_LIGHTS];

uniform sampler2D u_sampler_reflection;
uniform sampler2D u_sampler_refraction;
uniform sampler2D u_sampler_dudv;
uniform sampler2D u_sampler_normal;

uniform vec3 u_camera_pos;

uniform float u_time;

out vec4 out_color;

const float distortion_strength = 0.0075f;
const float time_factor = 0.01f;

vec4 calc_light(dir_light d_light, vec3 base_color, vec3 normal) {
    light base = d_light.base;
    vec3 direction = d_light.direction;

    vec3 pixel_to_cam = normalize(u_camera_pos - v_world_pos);
    vec3 reflection = normalize(reflect(direction, normal));
    float specular_base = max(dot(pixel_to_cam, reflection), 0.0f);
    
    const float specular_exponent = 20.0f;
    const float reflectivity = 0.4f;

    float specular = clamp(pow(specular_base, specular_exponent), 0.0f, 1.0f);
    vec4 specular_color = vec4(base.color, 1.0f) * vec4(base_color, 1.0f) * base.specular_intensity * specular * reflectivity;

    return specular_color;
}

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

    vec4 nmap = texture(u_sampler_normal, v_texcoord0 + d1 + d2);
    vec3 normal = vec3(nmap.r * 2.0f - 1.0f, nmap.b, nmap.g * 2.0f - 1.0f);

    float lighting = 1.0f;

    float fresnel = clamp(pow(dot(normalize(vec3(0, 1, 0)), normalize(u_camera_pos - v_world_pos)), 1.25f), 0.0f, 1.0f);

    // out_color = mix(reflect, refract, 0.5f);
    out_color = mix(mix(reflect, refract, fresnel), vec4(0.6f, 0.6f, 1.0f, 1.0f), 0.2f);
    if (u_num_dir_lights > 0) out_color += calc_light(u_dir_lights[0], out_color.rgb, normal);
}