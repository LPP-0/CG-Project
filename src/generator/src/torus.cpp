#define _USE_MATH_DEFINES
#include <cmath>
#include "../include/torus.hpp"
#include "../utils/writeVertices.hpp"
#include "structs.h"
#include <vector>

void generateTorus(float majorRadius, float minorRadius, int slices, int stacks, const std::string& filename) {
    std::vector<Point> vertices;

    float sliceStep = 2.0f * M_PI / slices;
    float stackStep = 2.0f * M_PI / stacks;

    for (int i = 0; i < slices; i++) {
        float theta1 = i * sliceStep;
        float theta2 = (i + 1) * sliceStep;

        float v1 = (float)i / slices;
        float v2 = (float)(i + 1) / slices;

        for (int j = 0; j < stacks; j++) {
            float phi1 = j * stackStep;
            float phi2 = (j + 1) * stackStep;

            float u1 = (float)j / stacks;
            float u2 = (float)(j + 1) / stacks;

            float cp1 = cos(phi1), sp1 = sin(phi1);
            float cp2 = cos(phi2), sp2 = sin(phi2);
            float ct1 = cos(theta1), st1 = sin(theta1);
            float ct2 = cos(theta2), st2 = sin(theta2);

            
            Point pA = {(majorRadius + minorRadius*cp1)*ct1, minorRadius*sp1, (majorRadius + minorRadius*cp1)*st1,  cp1*ct1, sp1, cp1*st1,  u1, v1};
            Point pB = {(majorRadius + minorRadius*cp1)*ct2, minorRadius*sp1, (majorRadius + minorRadius*cp1)*st2,  cp1*ct2, sp1, cp1*st2,  u2, v1};
            Point pC = {(majorRadius + minorRadius*cp2)*ct2, minorRadius*sp2, (majorRadius + minorRadius*cp2)*st2,  cp2*ct2, sp2, cp2*st2,  u2, v2};
            Point pD = {(majorRadius + minorRadius*cp2)*ct1, minorRadius*sp2, (majorRadius + minorRadius*cp2)*st1,  cp2*ct1, sp2, cp2*st1,  u1, v2};


            // Triângulo 1
            vertices.push_back(pA);
            vertices.push_back(pD);
            vertices.push_back(pC);

            // Triângulo 2
            vertices.push_back(pA);
            vertices.push_back(pC);
            vertices.push_back(pB);
        }
    }

    writeVerticesToFile(vertices, filename);
}