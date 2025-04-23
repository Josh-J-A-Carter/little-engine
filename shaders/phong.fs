#version 300 es

precision highp float;

const int MAX_POINT_LIGHTS = 100;
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

struct point_light {
    light base;
    vec3 world_pos;
    float attn_const;
    float attn_linear;
    float attn_exp;
};

struct material {
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
};

// Per-vertex data
in vec3 v_world_pos;
in vec2 v_texcoord0;
in vec3 v_normal;
in vec4 v_lightspace_pos;

// Lighting data
uniform int u_num_dir_lights;
uniform dir_light u_dir_lights[MAX_DIR_LIGHTS];
uniform int u_num_point_lights;
uniform point_light u_point_lights[MAX_POINT_LIGHTS];

// Per-model data
uniform material u_material;
uniform sampler2D u_sampler_diffuse;
uniform sampler2D u_sampler_specular;
uniform sampler2D u_sampler_shadow0;
uniform sampler2D u_sampler_noise;

uniform vec3 u_camera_pos;

// Output
out vec4 out_color;


vec4 calc_base_light(light base, vec3 direction, float shadow_factor) {
    vec4 ambient_color = vec4(base.color, 1.0f) * vec4(u_material.ambient_color, 1.0f) * base.ambient_intensity;

    float diffuse = clamp(dot(normalize(v_normal), -normalize(direction)), 0.0f, 1.0f);
    vec4 diffuse_color = vec4(base.color, 1.0f) * vec4(u_material.diffuse_color, 1.0f) * base.diffuse_intensity * diffuse;

    vec3 pixel_to_cam = normalize(u_camera_pos - v_world_pos);
    vec3 reflection = normalize(reflect(direction, v_normal)); //normalize(reflect(direction * 10.0f, v_normal * 10.0f));
    float specular_base = clamp(dot(pixel_to_cam, reflection), 0.0f, 1.0f);
    float specular_exponent = texture(u_sampler_specular, v_texcoord0).r * 255.0f;
    float specular = 0.0f;
    if (specular_exponent > 0.0f) specular = clamp(pow(specular_base, specular_exponent), 0.0f, 1.0f);
    
    vec4 specular_color = vec4(base.color, 1.0f) * vec4(u_material.specular_color, 1.0f) * base.specular_intensity * specular;

    return ambient_color + (diffuse_color + specular_color) * shadow_factor;
}

vec4 calc_point_light(point_light light) {
    vec3 dir = v_world_pos - light.world_pos;
    vec4 unscaled_emission = calc_base_light(light.base, dir, 1.0f);

    float d = length(dir); // * 10.0f;
    float scale_factor = light.attn_const + light.attn_linear * d + light.attn_exp * d * d;
    vec4 scaled_emission = unscaled_emission / scale_factor;

    return scaled_emission;
}

float calc_dir_light_shadow(vec3 light_direction) {
    vec2 uv = vec2(0.5f * v_lightspace_pos.x + 0.5f, 0.5f * v_lightspace_pos.y + 0.5f);
    float z = 0.5f * v_lightspace_pos.z + 0.5f;
    
    float diffuse = clamp(dot(normalize(v_normal), -normalize(light_direction)), 0.0f, 1.0f);
    // float bias = 0.0025f;
    float bias = mix(0.025f, 0.001f, diffuse);
    // return texture(u_sampler_shadow0, uv).x;

    float shadow = 0.0f;
    ivec2 t = textureSize(u_sampler_shadow0, 0);
    vec2 shadow_texel_size = vec2(1.0f / float(t.x), 1.0f / float(t.y));

    float rad = 2.0f;

    for (float x = -rad ; x <= rad ; x += 1.0f) {
        for (float y = -rad ; y <= rad ; y += 1.0f) {
            float depth = texture(u_sampler_shadow0, uv + vec2(x, y) * shadow_texel_size).x;
            shadow += z - bias > depth ? 0.0f : 1.0f;
        }
    }

    return shadow / ((2.0f * rad + 1.0f) * (2.0f * rad + 1.0f));
}

vec4 calc_dir_light(dir_light light) {
    return calc_base_light(light.base, light.direction, calc_dir_light_shadow(light.direction));
}

void main() {
    vec4 total_light = vec4(0.0f, 0.0f, 0.0f, 0.0f);

    for (int i = 0 ; i < u_num_dir_lights ; i += 1) {
        total_light += calc_dir_light(u_dir_lights[i]);
    }

    for (int i = 0 ; i < u_num_point_lights ; i += 1) {
        total_light += calc_point_light(u_point_lights[i]);
    }

    // float f = calc_dir_light_shadow(vec3(0.0f, 0.0f, 0.0f));
    // out_color = vec4(f, f, f, 1.0f);
    out_color = texture(u_sampler_diffuse, v_texcoord0) * total_light;
}