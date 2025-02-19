#version 450

layout(location = 0) out vec4 outColor; // Output color of the fragment

void main()
{
    // Set the fragment color to the input color
    outColor = vec4(1.0, 0.0, 0.0, 1.0);
}