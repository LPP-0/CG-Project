
#include "../include/plane.hpp"
#include "structs.h"
#include "../utils/writeVertices.hpp"

void generatePlane(float size, int divisions, const std::string& filename) {
    std::vector<Point> vertices;

    float start = -size / 2.0f;
    float step = size / divisions;

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            float x1 = start + j * step;
            float x2 = start + (j + 1) * step;
            float z1 = start + i * step;
            float z2 = start + (i + 1) * step;

            float u1 = (float)j / divisions;
            float u2 = (float)(j + 1) / divisions;
            float v1 = (float)i / divisions;
            float v2 = (float)(i + 1) / divisions;

            float nx = 0.0f, ny = 1.0f, nz = 0.0f;
            float nx_inv = 0.0f, ny_inv = -1.0f, nz_inv = 0.0f;

            // Triângulos face inferior (Normal para baixo)
            vertices.push_back({x1, 0.0f, z1, nx_inv, ny_inv, nz_inv, u1, v1});
            vertices.push_back({x2, 0.0f, z2, nx_inv, ny_inv, nz_inv, u2, v2});
            vertices.push_back({x1, 0.0f, z2, nx_inv, ny_inv, nz_inv, u1, v2});

            vertices.push_back({x1, 0.0f, z1, nx_inv, ny_inv, nz_inv, u1, v1});
            vertices.push_back({x2, 0.0f, z1, nx_inv, ny_inv, nz_inv, u2, v1});
            vertices.push_back({x2, 0.0f, z2, nx_inv, ny_inv, nz_inv, u2, v2});

            // Triângulos face superior (Normal para cima)
            vertices.push_back({x1, 0.0f, z1, nx, ny, nz, u1, v1});
            vertices.push_back({x1, 0.0f, z2, nx, ny, nz, u1, v2});
            vertices.push_back({x2, 0.0f, z2, nx, ny, nz, u2, v2});

            vertices.push_back({x1, 0.0f, z1, nx, ny, nz, u1, v1});
            vertices.push_back({x2, 0.0f, z2, nx, ny, nz, u2, v2});
            vertices.push_back({x2, 0.0f, z1, nx, ny, nz, u2, v1});
        }
    }
    writeVerticesToFile(vertices, filename);
}