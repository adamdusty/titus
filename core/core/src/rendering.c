#include "core/rendering.h"

#include <SDL3/SDL.h>
#include <cglm/cglm.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <titus/sdk.h>

CoreMesh core_create_capsule_mesh(float radius, float height, uint32_t slices, uint32_t stacks) {
    CoreMesh mesh;

    // Calculate vertex and index counts
    uint32_t hemisphere_stacks = stacks / 2;
    uint32_t cylinder_stacks   = 1; // The middle part

    // Total stacks = bottom hemisphere + cylinder + top hemisphere
    uint32_t total_stacks = hemisphere_stacks * 2 + cylinder_stacks;

    // Calculate vertex count
    // Each stack has (slices + 1) vertices, and we have (total_stacks + 1) rings of vertices
    mesh.vertex_count = (slices + 1) * (total_stacks + 1);

    // Calculate index count for triangles
    // Each grid cell has 2 triangles, each triangle has 3 indices
    // We have slices * total_stacks grid cells
    mesh.index_count = slices * total_stacks * 6;

    // Allocate memory
    mesh.vertices = (CoreVertexPositionNormal*)malloc(mesh.vertex_count * sizeof(CoreVertexPositionNormal));
    mesh.indices  = (uint32_t*)malloc(mesh.index_count * sizeof(uint32_t));

    if(!mesh.vertices || !mesh.indices) {
        // Handle allocation failure
        if(mesh.vertices)
            free(mesh.vertices);
        if(mesh.indices)
            free(mesh.indices);

        mesh.vertices     = NULL;
        mesh.indices      = NULL;
        mesh.vertex_count = 0;
        mesh.index_count  = 0;

        return mesh;
    }

    const float PI          = 3.14159265358979323846f;
    const float half_height = height * 0.5f;
    int vertex_index        = 0;
    int index_index         = 0;

    // Generate vertices
    for(uint32_t i = 0; i <= total_stacks; i++) {
        float stack_perc = (float)i / (float)total_stacks;
        float theta      = stack_perc * PI; // Range from 0 to PI

        float y;
        float stack_radius;

        // Bottom hemisphere
        if(i < hemisphere_stacks) {
            float hemi_perc = (float)i / (float)hemisphere_stacks;
            theta           = (-0.5f * PI) + (hemi_perc * (PI / 2.0f)); // Range from -PI/2 to 0
            y               = -half_height + radius * sinf(theta);      // Using sin for correct orientation
            stack_radius    = radius * cosf(theta);                     // Using cos for correct orientation
        }
        // Cylinder
        else if(i <= hemisphere_stacks + cylinder_stacks) {
            y            = -half_height + (float)(i - hemisphere_stacks) * height / (float)cylinder_stacks;
            stack_radius = radius;
        }
        // Top hemisphere
        else {
            float hemi_perc = (float)(i - hemisphere_stacks - cylinder_stacks) / (float)hemisphere_stacks;
            theta           = hemi_perc * (PI / 2.0f); // Range from PI/2 to 0
            y               = half_height + radius * cosf(theta);
            stack_radius    = radius * sinf(theta);
        }

        for(uint32_t j = 0; j <= slices; j++) {
            float slice_perc = (float)j / (float)slices;
            float phi        = slice_perc * 2.0f * PI; // Range from 0 to 2PI

            float x = stack_radius * cosf(phi);
            float z = stack_radius * sinf(phi);

            mesh.vertices[vertex_index].position[0] = x;
            mesh.vertices[vertex_index].position[1] = y;
            mesh.vertices[vertex_index].position[2] = z;

            /* I have no idea if these normals are correct. I'm going to add lighting since I have to do that anyway. */
            // Calculate normals
            if(i < hemisphere_stacks) {
                // Bottom hemisphere normals
                mesh.vertices[vertex_index].normal[0] = cosf(theta) * cosf(phi);
                mesh.vertices[vertex_index].normal[1] = -sinf(phi);
                mesh.vertices[vertex_index].normal[2] = cosf(phi) * sinf(theta);
            } else if(i <= hemisphere_stacks + cylinder_stacks) {
                // Cylinder normals
                mesh.vertices[vertex_index].normal[0] = cosf(theta);
                mesh.vertices[vertex_index].normal[1] = 0.0f;
                mesh.vertices[vertex_index].normal[2] = sinf(phi);
            } else {
                // Top hemisphere normals
                mesh.vertices[vertex_index].normal[0] = cosf(theta) * cosf(phi);
                mesh.vertices[vertex_index].normal[1] = sinf(phi);
                mesh.vertices[vertex_index].normal[2] = cosf(phi) * sinf(theta);
            }
            /* ************************************************************* */

            vertex_index++;
        }
    }

    // Generate indices
    for(uint32_t i = 0; i < total_stacks; i++) {
        for(uint32_t j = 0; j < slices; j++) {
            uint32_t first  = i * (slices + 1) + j;
            uint32_t second = first + (slices + 1);

            // First triangle
            mesh.indices[index_index++] = first;
            mesh.indices[index_index++] = second;
            mesh.indices[index_index++] = first + 1;

            // Second triangle
            mesh.indices[index_index++] = first + 1;
            mesh.indices[index_index++] = second;
            mesh.indices[index_index++] = second + 1;
        }
    }

    return mesh;
}

CoreMesh core_create_cube_mesh(float width, float height, float depth) {
    CoreMesh mesh;

    // Define the 8 unique vertex positions of the cube
    vec3 positions[] = {
        {-width, -height, -depth}, // 0
        {width, -height, -depth},  // 1
        {width, height, -depth},   // 2
        {-width, height, -depth},  // 3
        {-width, -height, depth},  // 4
        {width, -height, depth},   // 5
        {width, height, depth},    // 6
        {-width, height, depth}    // 7
    };

    // Define the 6 face normals of the cube
    vec3 normals[] = {
        {0.0f, 0.0f, -1.0f}, // Back face
        {0.0f, 0.0f, 1.0f},  // Front face
        {-1.0f, 0.0f, 0.0f}, // Left face
        {1.0f, 0.0f, 0.0f},  // Right face
        {0.0f, -1.0f, 0.0f}, // Bottom face
        {0.0f, 1.0f, 0.0f}   // Top face
    };

    // Define the 12 triangles (2 per face) using the 8 unique vertices

    // clang-format off
    uint32_t face_indices[] = {
        0,1,2,2,3,0, // Back face
        4,5,6,6,7,4,// Front face
        0,3,7,7,4,0,// Left face
        1,5,6,6,2,1,// Right face
        0,1,5,5,4,0,// Bottom face
        3,2,6,6,7,3,// Top face
    };
    // clang-format on

    // For flat shading, each face has its own set of vertices with the same normal
    mesh.vertex_count = 36; // 6 faces * 6 vertices per face (2 triangles)
    mesh.vertices     = (CoreVertexPositionNormal*)malloc(mesh.vertex_count * sizeof(CoreVertexPositionNormal));

    mesh.index_count = 36; // 12 triangles * 3 indices per triangle
    mesh.indices     = (uint32_t*)malloc(mesh.index_count * sizeof(uint32_t));

    // Create vertices and assign normals
    for(size_t i = 0; i < mesh.index_count; ++i) {
        uint32_t vertex_index = face_indices[i];
        memcpy(mesh.vertices[i].position, positions[vertex_index], sizeof(vec3));

        // Assign the normal based on the face
        if(i < 6) {
            memcpy(mesh.vertices[i].normal, normals[0], sizeof(vec3)); // Back face
        } else if(i < 12) {
            memcpy(mesh.vertices[i].normal, normals[1], sizeof(vec3)); // Front face
        } else if(i < 18) {
            memcpy(mesh.vertices[i].normal, normals[2], sizeof(vec3)); // Left face
        } else if(i < 24) {
            memcpy(mesh.vertices[i].normal, normals[3], sizeof(vec3)); // Right face
        } else if(i < 30) {
            memcpy(mesh.vertices[i].normal, normals[4], sizeof(vec3)); // Bottom face
        } else {
            memcpy(mesh.vertices[i].normal, normals[5], sizeof(vec3)); // Top face
        }

        // Assign indices (just a linear sequence for flat shading)
        mesh.indices[i] = i;
    }

    return mesh;
}

// Function to free a CoreMesh
void core_free_core_mesh(CoreMesh* mesh) {
    if(mesh) {
        if(mesh->vertices) {
            free(mesh->vertices);
            mesh->vertices = NULL;
        }
        if(mesh->indices) {
            free(mesh->indices);
            mesh->indices = NULL;
        }
        mesh->vertex_count = 0;
        mesh->index_count  = 0;
    }
}
