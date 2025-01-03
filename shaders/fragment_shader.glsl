#version 410 core

struct light {
    vec3 pos;
    vec3 color;
    float ambient_intensity;
    float diffuse_intensity;
};

struct material {
    vec3 ambient_color;
    vec3 diffuse_color;
};

in vec3 v_world_pos;
in vec2 v_texcoord0;
in vec3 v_normal;

uniform light u_light;
uniform material u_material;
uniform sampler2D u_sampler;

out vec4 out_color;

void main() {
    vec4 ambient_color = vec4(u_light.color, 1.0f) * vec4(u_material.ambient_color, 1.0f) * u_light.ambient_intensity;

    vec3 light_dir = v_world_pos - u_light.pos;

    float distance = length(light_dir) * 3;
    float diffuse = clamp(dot(normalize(v_normal), -normalize(light_dir)), 0.0f, 1.0f) / (1 + distance) * 3;

    vec4 diffuse_color = vec4(u_light.color, 1.0f) * vec4(u_material.diffuse_color, 1.0f) * u_light.diffuse_intensity * diffuse;

    out_color = texture2D(u_sampler, v_texcoord0) * (ambient_color + diffuse_color);
}