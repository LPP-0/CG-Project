#define _USE_MATH_DEFINES
#include <cmath>

#include "../include/cylinder.hpp"
#include "structs.h"
#include "../utils/writeVertices.hpp"

void generateCylinder(float radius, float height, int slices,const std::string& filename){
    std::vector<Point> vertices;
    float sliceStep = 2.0f * M_PI / slices;
    float half = height / 2.0f;

   for(int i=0; i< slices; i++){
        float theta1  = i * sliceStep;
        float theta2 = (i + 1) * sliceStep;

        float u1 = (float)i / slices;
        float u2 = (float)(i + 1) / slices;
        
        float x1 = sin(theta1); float z1 = cos(theta1);
        float x2 = sin(theta2); float z2 = cos(theta2);

        // Pontos da Lateral (Normal radial: x, 0, z)
        Point pA = {radius * x1,  half, radius * z1, x1, 0.0f, z1, u1, 1.0f};
        Point pB = {radius * x2,  half, radius * z2, x2, 0.0f, z2, u2, 1.0f};
        Point pC = {radius * x2, -half, radius * z2, x2, 0.0f, z2, u2, 0.0f};
        Point pD = {radius * x1, -half, radius * z1, x1, 0.0f, z1, u1, 0.0f};

        // --- Base Inferior (Normal: 0, -1, 0) ---
        float uv_u1 = 0.5f + 0.5f * x1; float uv_v1 = 0.5f + 0.5f * z1;
        float uv_u2 = 0.5f + 0.5f * x2; float uv_v2 = 0.5f + 0.5f * z2;
        
        vertices.push_back({0.0f, -half, 0.0f, 0.0f, -1.0f, 0.0f, 0.5f, 0.5f});
        vertices.push_back({radius * x2, -half, radius * z2, 0.0f, -1.0f, 0.0f, uv_u2, uv_v2});
        vertices.push_back({radius * x1, -half, radius * z1, 0.0f, -1.0f, 0.0f, uv_u1, uv_v1});

        // --- Base Superior (Normal: 0, 1, 0) ---
        vertices.push_back({0.0f, half, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f});
        vertices.push_back({radius * x1, half, radius * z1, 0.0f, 1.0f, 0.0f, uv_u1, uv_v1});
        vertices.push_back({radius * x2, half, radius * z2, 0.0f, 1.0f, 0.0f, uv_u2, uv_v2});
    
    
        // Lateral - Triângulo 1 (A, D, C)
        vertices.push_back(pA);
        vertices.push_back(pD);
        vertices.push_back(pC);
        

        // Lateral - Triângulo 2 (A, C, B)
        vertices.push_back(pA);
        vertices.push_back(pC);
        vertices.push_back(pB);

    }

    writeVerticesToFile(vertices, filename);

}