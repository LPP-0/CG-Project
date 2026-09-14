#define _USE_MATH_DEFINES
#include "../include/comet_patch.hpp"
#include "structs.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>


Point spherePoint(float u, float v) {
    float theta = u * M_PI;
    float phi   = v * 2.0f * M_PI;

    Point p;
    p.x = sin(theta) * cos(phi);
    p.y = cos(theta);
    p.z = sin(theta) * sin(phi);

    return p;
}

float wrappedVDistance(float v, float centerV) {
    float d = fabsf(v - centerV);
    return std::min(d, 1.0f - d);
}


float rockyNoise(float u, float v) {
    float n = 0.0f;
    n += 0.08f * sinf(3.0f * M_PI * u) * cosf(4.0f * M_PI * v);
    n += 0.05f * sinf(5.0f * M_PI * u + 1.0f) * sinf(6.0f * M_PI * v + 2.0f);
    n += 0.03f * cosf(9.0f * M_PI * u) * sinf(11.0f * M_PI * v);
    n += 0.008f * sinf(21.0f * M_PI * u) * cosf(25.0f * M_PI * v);
    return n;
}


float calcCrater(float u, float v, float cu, float cv, float radius, float depth, float rimHeight) {
    float du = u - cu;
    float dv = wrappedVDistance(v, cv);

    // Corrige distorção perto dos polos para crateras mais circulares.
    dv *= sinf(cu * M_PI);

    float dist = sqrtf(du * du + dv * dv);
    if (dist > radius) return 0.0f;

    float x = dist / radius; 

    float pitCore = -depth * expf(-powf(x / 0.34f, 2.6f)); // centro (mais fundo)

    float innerWall = -0.30f * depth * (1.0f - x) * (1.0f - x); // parede interior (suave subida)

    float rim = rimHeight * expf(-powf((x - 0.86f) / 0.10f, 2.0f)); // borda (pico perto de x = 0.86)

    float falloff = 1.0f - x * x * x;

    return (pitCore + innerWall + rim) * falloff;
}

Point deform(Point p, float u, float v) {
    p.x *= 1.18f;
    p.y *= 0.88f;
    p.z *= 0.98f;

    float radial = 1.0f;

    radial += rockyNoise(u, v);

    // Formação de crateras

    // Face principal
    radial += calcCrater(u, v, 0.33f, 0.20f, 0.13f, 0.38f, 0.12f);
    radial += calcCrater(u, v, 0.50f, 0.40f, 0.09f, 0.26f, 0.08f);
    radial += calcCrater(u, v, 0.24f, 0.74f, 0.14f, 0.20f, 0.06f);

    // Lado oposto
    radial += calcCrater(u, v, 0.67f, 0.66f, 0.20f, 0.40f, 0.12f);
    radial += calcCrater(u, v, 0.76f, 0.14f, 0.10f, 0.24f, 0.08f);

    // Micro-impactos
    radial += calcCrater(u, v, 0.57f, 0.84f, 0.055f, 0.14f, 0.05f);
    radial += calcCrater(u, v, 0.82f, 0.40f, 0.060f, 0.11f, 0.04f);
    radial += calcCrater(u, v, 0.30f, 0.56f, 0.055f, 0.10f, 0.03f);

    // Prevenir deformações extremas
    radial = std::max(0.25f, std::min(1.65f, radial));

    p.x *= radial;
    p.y *= radial;
    p.z *= radial;

    p.x += 0.15f * sinf(u * M_PI);

    return p;
}


void generateComet(const std::string& outputFile){

    const int grid = 19;
    const int totalPoints = grid * grid;

    std::vector<Point> points;
    points.reserve(totalPoints);

    // 1. Gerar pontos de controlo em espaço paramétrico.
    for (int i = 0; i < grid; i++) {
        Point firstPointInRow = {0.0f, 0.0f, 0.0f};

        for (int j = 0; j < grid; j++) {

            if (j == grid - 1) {
                points.push_back(firstPointInRow);
                continue;
            }

            float u = float(i) / (grid - 1);
            float v = float(j) / (grid - 1);

            Point p = spherePoint(u, v);
            p = deform(p, u, v);

            if (j == 0) {
                firstPointInRow = p;
            }

            points.push_back(p);
        }
    }

    // 2. Agrupar os pontos em patches cúbicos de 4x4.
    std::vector<std::vector<int>> patches;

    for (int i = 0; i < grid - 3; i += 3) {
        for (int j = 0; j < grid - 3; j += 3) {

            std::vector<int> patch;

            for (int u = 0; u < 4; u++) {
                for (int v = 0; v < 4; v++) {
                    int idx = (i + u) * grid + (j + v);
                    patch.push_back(idx);
                }
            }

            patches.push_back(patch);
        }
    }

    // 3. Escrever o ficheiro .patch
    std::ofstream file(outputFile);

    if (!file.is_open()) {
        std::cerr << "Error opening output file: " << outputFile << "\n";
        return;
    }

    file << patches.size() << "\n";

    for (auto& p : patches) {
        for (int i = 0; i < 16; i++) {
            file << p[i];
            if (i != 15) file << ", ";
        }
        file << "\n";
    }

    file << points.size() << "\n";

    for (auto& p : points) {
        file << p.x << ", " << p.y << ", " << p.z << "\n";
    }

    file.close();

}