#version 410 core

struct light {
    vec3 color;
    float ambient_intensity;
};

struct material {
    vec3 ambient_color;
};

in vec2 v_texcoord0;

uniform light u_light;
uniform material u_material;
uniform sampler2D u_sampler;

out vec4 out_colour;

void main() {
    out_colour = texture2D(u_sampler, v_texcoord0) * vec4(u_light.color, 1.0f) * u_light.ambient_intensity * vec4(u_material.ambient_color, 1.0f);
}