#version 410 core

struct light {
    vec3 direction;
    vec3 color;
    float ambient_intensity;
    float diffuse_intensity;
    float specular_intensity;
};

struct material {
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
};

in vec3 v_world_pos;
in vec2 v_texcoord0;
in vec3 v_normal;

uniform light u_dir_light;
uniform material u_material;

uniform sampler2D u_sampler_diffuse;
uniform sampler2D u_sampler_specular;
uniform vec3 u_camera_pos;

out vec4 out_color;

// void point_light() {
//     vec4 ambient_color = vec4(u_light.color, 1.0f) * vec4(u_material.ambient_color, 1.0f) * u_light.ambient_intensity;

//     vec3 light_dir = v_world_pos - u_light.pos;

//     float distance = length(light_dir) * 3;
//     float diffuse = clamp(dot(normalize(v_normal), -normalize(light_dir)), 0.0f, 1.0f) / (1 + distance) * 3;

//     vec4 diffuse_color = vec4(u_light.color, 1.0f) * vec4(u_material.diffuse_color, 1.0f) * u_light.diffuse_intensity * diffuse;

//     out_color = texture2D(u_sampler, v_texcoord0) * (ambient_color + diffuse_color);
// }

void main() {
    vec4 ambient_color = vec4(u_dir_light.color, 1.0f) * vec4(u_material.ambient_color, 1.0f) * u_dir_light.ambient_intensity;

    float diffuse = clamp(dot(normalize(v_normal), -normalize(u_dir_light.direction)), 0.0f, 1.0f);
    vec4 diffuse_color = vec4(u_dir_light.color, 1.0f) * vec4(u_material.diffuse_color, 1.0f) * u_dir_light.diffuse_intensity * diffuse;

    vec3 pixel_to_cam = normalize(u_camera_pos - v_world_pos);
    vec3 reflection = normalize(reflect(u_dir_light.direction*10, v_normal*10));
    float specular_base = dot(pixel_to_cam, reflection);
    float specular_exponent = texture2D(u_sampler_specular, v_texcoord0).r * 255.0f;
    float specular = 0;
    if (specular_exponent > 0) specular = clamp(pow(specular_base, specular_exponent), 0, 1);
    
    vec4 specular_color = vec4(u_dir_light.color, 1.0f) * vec4(u_material.specular_color, 1.0f) * u_dir_light.specular_intensity * specular;

    out_color = texture2D(u_sampler_diffuse, v_texcoord0) * (ambient_color + diffuse_color + specular_color);
}