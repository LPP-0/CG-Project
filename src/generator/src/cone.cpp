#define _USE_MATH_DEFINES
#include <cmath>
#include "../include/cone.hpp"
#include "structs.h"
#include "../utils/writeVertices.hpp"

void generateCone(float radius, float height, int slices, int stacks,const std::string& filename){
    std::vector<Point> vertices;
    float sliceStep = 2.0f * M_PI / slices;
    float stackStep = height / stacks;

    // Fator para a inclinação da normal no corpo
    float cos_alpha = height / sqrt(height * height + radius * radius);
    float sin_alpha = radius / sqrt(height * height + radius * radius);

   for (int i = 0; i < stacks; i++) {
        // Cálculo dos raios e alturas para os dois níveis da stack
        float r1 = radius * (1.0f - (float)i / stacks);
        float r2 = radius * (1.0f - (float)(i + 1) / stacks);
        float y1 = i * stackStep;
        float y2 = (i + 1) * stackStep;

        float v1 = (float)i / stacks;
        float v2 = (float)(i + 1) / stacks;


        for (int j = 0; j < slices; j++) {
            float theta1 = j * sliceStep;
            float theta2 = (j + 1) * sliceStep;

            float u1 = (float)j / slices;
            float u2 = (float)(j + 1) / slices;

            // Valores de sin/cos para as normais
            float s1 = sin(theta1); float c1 = cos(theta1);
            float s2 = sin(theta2); float c2 = cos(theta2);

            // A normal no corpo de um cone é (sin(theta)*cos_alpha, sin_alpha, cos(theta)*cos_alpha)
            Point pA = {r2 * s1, y2, r2 * c1, s1 * cos_alpha, sin_alpha, c1 * cos_alpha, u1, v2};
            Point pB = {r2 * s2, y2, r2 * c2, s2 * cos_alpha, sin_alpha, c2 * cos_alpha, u2, v2};
            Point pC = {r1 * s2, y1, r1 * c2, s2 * cos_alpha, sin_alpha, c2 * cos_alpha, u2, v1};
            Point pD = {r1 * s1, y1, r1 * c1, s1 * cos_alpha, sin_alpha, c1 * cos_alpha, u1, v1};

            // Se estivermos na última stack (ápice), r2 é 0.
            // Os pontos pA e pB coicidem no ponto (0, height, 0).
            if (i == stacks - 1) {
                // Apenas o triângulo T2 é necessário (forma a ponta do cone)
                vertices.push_back(pB); // Este é o ápice (0, H, 0)
                vertices.push_back(pD);
                vertices.push_back(pC);
            } 
            else {
                // Caso geral: Construção de ambos os triângulos (Corpo do cone)
                // T1
                vertices.push_back(pA);
                vertices.push_back(pD);
                vertices.push_back(pB);
                // T2
                vertices.push_back(pB);
                vertices.push_back(pD);
                vertices.push_back(pC);
            }
        }
    }
    // Base
    for (int k = 0; k < slices; k++) {
        float theta1 = k * sliceStep;
        float theta2 = (k + 1) * sliceStep;

        Point pA = {radius * sin(theta1), 0, radius * cos(theta1)};
        Point pB = {radius * sin(theta2), 0, radius * cos(theta2)};

        float s1 = sin(theta1); float c1 = cos(theta1);
        float s2 = sin(theta2); float c2 = cos(theta2);

        vertices.push_back({0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.5f, 0.5f});
        vertices.push_back({radius * s2, 0.0f, radius * c2, 0.0f, -1.0f, 0.0f, 0.5f + 0.5f * s2, 0.5f + 0.5f * c2});
        vertices.push_back({radius * s1, 0.0f, radius * c1, 0.0f, -1.0f, 0.0f, 0.5f + 0.5f * s1, 0.5f + 0.5f * c1});
    }

    writeVerticesToFile(vertices, filename);

}