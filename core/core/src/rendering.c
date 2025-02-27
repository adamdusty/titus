#include "core/rendering.h"

#include <SDL3/SDL.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <titus/sdk.h>

CoreMesh core_create_capsule_mesh(float radius, float height, int slices, int stacks) {
    CoreMesh mesh;

    // Calculate vertex and index counts
    int hemisphere_stacks = stacks / 2;
    int cylinder_stacks   = 1; // The middle part

    // Total stacks = bottom hemisphere + cylinder + top hemisphere
    int total_stacks = hemisphere_stacks * 2 + cylinder_stacks;

    // Calculate vertex count
    // Each stack has (slices + 1) vertices, and we have (total_stacks + 1) rings of vertices
    mesh.vertex_count = (slices + 1) * (total_stacks + 1);

    // Calculate index count for triangles
    // Each grid cell has 2 triangles, each triangle has 3 indices
    // We have slices * total_stacks grid cells
    mesh.index_count = slices * total_stacks * 6;

    // Allocate memory
    mesh.vertices = (CoreVertexPosition*)malloc(mesh.vertex_count * sizeof(CoreVertexPosition));
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
    for(int i = 0; i <= total_stacks; i++) {
        float stack_perc = (float)i / (float)total_stacks;
        float theta      = stack_perc * PI; // Range from 0 to PI

        float y;
        float stack_radius;

        // Bottom hemisphere
        if(i < hemisphere_stacks) {
            float hemi_perc = (float)i / (float)hemisphere_stacks;
            // theta           = (1.0f - hemi_perc) * (PI / 2.0f) + (PI / 2.0f); // Range from PI to PI/2
            theta = (-0.5f * PI) + (hemi_perc * (PI / 2.0f)); // Range from -PI/2 to 0
            // y               = -half_height - radius * cosf(theta);
            // stack_radius    = radius * sinf(theta);
            y            = -half_height + radius * sinf(theta); // Using sin for correct orientation
            stack_radius = radius * cosf(theta);                // Using cos for correct orientation
        }
        // Cylinder
        else if(i <= hemisphere_stacks + cylinder_stacks) {
            y            = -half_height + (i - hemisphere_stacks) * height / cylinder_stacks;
            stack_radius = radius;
        }
        // Top hemisphere
        else {
            float hemi_perc = (float)(i - hemisphere_stacks - cylinder_stacks) / (float)hemisphere_stacks;
            theta           = hemi_perc * (PI / 2.0f); // Range from PI/2 to 0
            y               = half_height + radius * cosf(theta);
            stack_radius    = radius * sinf(theta);
        }

        for(int j = 0; j <= slices; j++) {
            float slice_perc = (float)j / (float)slices;
            float phi        = slice_perc * 2.0f * PI; // Range from 0 to 2PI

            float x = stack_radius * cosf(phi);
            float z = stack_radius * sinf(phi);

            mesh.vertices[vertex_index].x = x;
            mesh.vertices[vertex_index].y = y;
            mesh.vertices[vertex_index].z = z;

            vertex_index++;
        }
    }

    // Generate indices
    for(int i = 0; i < total_stacks; i++) {
        for(int j = 0; j < slices; j++) {
            int first  = i * (slices + 1) + j;
            int second = first + (slices + 1);

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
