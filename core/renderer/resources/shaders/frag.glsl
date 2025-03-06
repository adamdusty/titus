#version 450

layout(location = 0) in vec3 in_norm;

layout(location = 0) out vec4 outColor; // Output color of the fragment

void main()
{
    vec3 light_dir = normalize(vec3(-1.0, -1.0, -1.0));

    vec3 obj_color = vec3(1.0, 1.0, 0.0);
    vec3 norm = normalize(in_norm);


    float ambientStrength = 0.5;
    float ambient = ambientStrength;

    float diffuse = max(dot(norm, -light_dir), 0.0);

    vec3 result = (ambient + diffuse) * obj_color;
    outColor = vec4(result, 1.0);
}