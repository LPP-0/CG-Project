#define _USE_MATH_DEFINES
#include <cmath>
#include "../include/sphere.hpp"
#include "structs.h"
#include "../utils/writeVertices.hpp"

void generateSphere(float radius, int slices, int stacks, const std::string& filename) {
	std::vector<Point> vertices;
    float sliceStep = 2.0f * M_PI / slices;
    float stackStep = M_PI / stacks;

    for (int i = 0; i < stacks; i++) {
        float phi1 = M_PI / 2.0f - i * stackStep;
        float phi2 = M_PI / 2.0f - (i + 1) * stackStep;

        float v1 = (float)i / stacks;
        float v2 = (float)(i + 1) / stacks;

        for (int j = 0; j < slices; j++) {
            float theta1 = j * sliceStep;
            float theta2 = (j + 1) * sliceStep;

            float u1 = (float)j / slices;
            float u2 = (float)(j + 1) / slices;

            //Posições dos vértices para a célula atual
            float xA = cos(phi1) * sin(theta1); float yA = sin(phi1); float zA = cos(phi1) * cos(theta1);
            float xB = cos(phi1) * sin(theta2); float yB = sin(phi1); float zB = cos(phi1) * cos(theta2);
            float xC = cos(phi2) * sin(theta2); float yC = sin(phi2); float zC = cos(phi2) * cos(theta2);
            float xD = cos(phi2) * sin(theta1); float yD = sin(phi2); float zD = cos(phi2) * cos(theta1);


            // Pontos calculados para a célula atual
            Point pA = {radius * xA, radius * yA, radius * zA, xA, yA, zA, u1, v1};
            Point pB = {radius * xB, radius * yB, radius * zB, xB, yB, zB, u2, v1};
            Point pC = {radius * xC, radius * yC, radius * zC, xC, yC, zC, u2, v2};
            Point pD = {radius * xD, radius * yD, radius * zD, xD, yD, zD, u1, v2 };

            // 1. CASO POLO NORTE: Apenas o triângulo de baixo (T2) é válido
            if (i == 0) {
                // pA e pB são o mesmo ponto (0, radius, 0)
                vertices.push_back(pB);
                vertices.push_back(pD);
                vertices.push_back(pC);
            }
            // 2. CASO POLO SUL: Apenas o triângulo de cima (T1) é válido
            else if (i == stacks - 1) {
                // pC e pD são o mesmo ponto (0, -radius, 0)
                vertices.push_back(pA);
                vertices.push_back(pD);
                vertices.push_back(pB);
            }
            // 3.CORPO DA ESFERA: Desenha ambos os triângulos
            else {
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

    writeVerticesToFile(vertices, filename);
}