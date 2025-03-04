#version 450

layout(location = 0) in vec3 inPosition;

layout(set = 1, binding = 0) uniform u_camera{
    mat4 proj;
    mat4 view;
} cam;


void main()
{
    gl_Position = cam.proj * cam.view * vec4(inPosition, 1.0);
}