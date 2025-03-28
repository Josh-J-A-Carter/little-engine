#version 300 es

in vec3 in_position;
in vec2 in_texcoord0;
in vec3 in_normal;

uniform mat4 u_model_matrix;

uniform mat4 u_shadow_matrix;

void main() {
    gl_Position = u_shadow_matrix * u_model_matrix * vec4(in_position, 1.0f);
    // gl_Position = u_model_matrix * vec4(in_position, 1.0f);
    // gl_Position.z = 0.0f;
    // gl_Position = vec4(in_position.x, in_position.y, 0.0f, 1.0f);
}