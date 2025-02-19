#version 450

layout(location = 0) in vec4 inPosition; // Vertex position in object space

void main()
{
    // Transform the vertex position to clip space
    gl_Position = inPosition;
}