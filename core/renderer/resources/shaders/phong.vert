#version 450

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_norm;

layout(location = 0) out vec3 out_norm;

layout(set = 1, binding = 0) uniform u_camera{
    mat4 proj;
    mat4 view;
} cam;


void main()
{
    gl_Position = cam.proj * cam.view * vec4(in_pos, 1.0);
    out_norm = in_norm;
}