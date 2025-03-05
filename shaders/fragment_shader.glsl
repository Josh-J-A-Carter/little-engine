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

// Lighting data
uniform int u_num_dir_lights;
uniform dir_light u_dir_lights[MAX_DIR_LIGHTS];
uniform int u_num_point_lights;
uniform point_light u_point_lights[MAX_POINT_LIGHTS];

// Per-model data
uniform material u_material;
uniform sampler2D u_sampler_diffuse;
uniform sampler2D u_sampler_specular;
uniform vec3 u_camera_pos;

// Output
out vec4 out_color;


vec4 calc_base_light(light base, vec3 direction) {
    vec4 ambient_color = vec4(base.color, 1.0f) * vec4(u_material.ambient_color, 1.0f) * base.ambient_intensity;

    float diffuse = clamp(dot(normalize(v_normal), -normalize(direction)), 0.0f, 1.0f);
    vec4 diffuse_color = vec4(base.color, 1.0f) * vec4(u_material.diffuse_color, 1.0f) * base.diffuse_intensity * diffuse;

    vec3 pixel_to_cam = normalize(u_camera_pos - v_world_pos);
    vec3 reflection = normalize(reflect(direction * 10.0f, v_normal * 10.0f));
    float specular_base = clamp(dot(pixel_to_cam, reflection), 0.0f, 1.0f);
    float specular_exponent = texture(u_sampler_specular, v_texcoord0).r * 255.0f;
    float specular = 0.0f;
    if (specular_exponent > 0.0f) specular = clamp(pow(specular_base, specular_exponent), 0.0f, 1.0f);
    
    vec4 specular_color = vec4(base.color, 1.0f) * vec4(u_material.specular_color, 1.0f) * base.specular_intensity * specular;

    return ambient_color + diffuse_color + specular_color;
}

vec4 calc_point_light(point_light light) {
    vec3 dir = v_world_pos - light.world_pos;
    vec4 unscaled_emission = calc_base_light(light.base, dir);

    float d = length(dir) * 10.0f;
    float scale_factor = light.attn_const + light.attn_linear * d + light.attn_exp * d * d;
    vec4 scaled_emission = unscaled_emission / scale_factor;

    return scaled_emission;
}

vec4 calc_dir_light(dir_light light) {
    return calc_base_light(light.base, light.direction);
}

void main() {
    vec4 total_light = vec4(0.0f, 0.0f, 0.0f, 0.0f);

    for (int i = 0 ; i < u_num_dir_lights ; i += 1) {
        total_light += calc_dir_light(u_dir_lights[i]);
    }

    for (int i = 0 ; i < u_num_point_lights ; i += 1) {
        total_light += calc_point_light(u_point_lights[i]);
    }

    out_color = texture(u_sampler_diffuse, v_texcoord0) * total_light;
}