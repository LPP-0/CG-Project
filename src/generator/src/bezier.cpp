#define NOMINMAX // windows correção
#define NOMINMAX

#include "../include/bezier.hpp"
#include "../utils/writeVertices.hpp"
#include "structs.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <limits>
#include <array>

struct BezierPatch {
    Point controlPoints[4][4];
};

void bernstein3(float t, float B[4]) {
    float it = 1.0f - t;
    B[0] = it * it * it;
    B[1] = 3.0f * t * it * it;
    B[2] = 3.0f * t * t * it;
    B[3] = t * t * t;
}

void bernstein3Deriv(float t, float dB[4]) {
    float it = 1.0f - t;
    dB[0] = -3.0f * it * it;
    dB[1] = 3.0f * it * it - 6.0f * t * it;
    dB[2] = 6.0f * t * it - 3.0f * t * t;
    dB[3] = 3.0f * t * t;
}

Point evaluateBezierPatch(const BezierPatch& patch, float u, float v) {
    Point result = {0.0f, 0.0f, 0.0f};
    float Bu[4], Bv[4];
    bernstein3(u, Bu);
    bernstein3(v, Bv);
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) {
            float coeff = Bu[i] * Bv[j];
            result.x += coeff * patch.controlPoints[i][j].x;
            result.y += coeff * patch.controlPoints[i][j].y;
            result.z += coeff * patch.controlPoints[i][j].z;
        }
    return result;
}

Point evaluateBezierPatchDu(const BezierPatch& patch, float u, float v) {
    Point result = {0.0f, 0.0f, 0.0f};
    float dBu[4], Bv[4];
    bernstein3Deriv(u, dBu);
    bernstein3(v, Bv);
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) {
            float coeff = dBu[i] * Bv[j];
            result.x += coeff * patch.controlPoints[i][j].x;
            result.y += coeff * patch.controlPoints[i][j].y;
            result.z += coeff * patch.controlPoints[i][j].z;
        }
    return result;
}

Point evaluateBezierPatchDv(const BezierPatch& patch, float u, float v) {
    Point result = {0.0f, 0.0f, 0.0f};
    float Bu[4], dBv[4];
    bernstein3(u, Bu);
    bernstein3Deriv(v, dBv);
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) {
            float coeff = Bu[i] * dBv[j];
            result.x += coeff * patch.controlPoints[i][j].x;
            result.y += coeff * patch.controlPoints[i][j].y;
            result.z += coeff * patch.controlPoints[i][j].z;
        }
    return result;
}

static void normalize(Point& n) {
    float len = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
    if (len > 0.0f) {
        n.x /= len;
        n.y /= len;
        n.z /= len;
    }
}

std::vector<BezierPatch> readBezierPatches(const std::string& filename) {
    std::vector<BezierPatch> patches;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening control points file: " << filename << "\n";
        return patches;
    }

    int patchCount;
    file >> patchCount;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::vector<std::array<int, 16>> patchIndexes;
    patchIndexes.reserve(patchCount);

    for (int p = 0; p < patchCount; ++p) {
        std::string line;
        std::getline(file, line);
        if (line.find_first_not_of(" \t\r\n") == std::string::npos) {
            --p;
            continue;
        }

        std::array<int, 16> indexes{};
        std::stringstream lineStream(line);
        for (int i = 0; i < 16; ++i) {
            std::string token;
            if (!std::getline(lineStream, token, ',')) {
                return patches;
            }
            indexes[i] = std::stoi(token);
        }
        patchIndexes.push_back(indexes);
    }

    int controlPointCount = 0;
    file >> controlPointCount;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::vector<Point> controlPoints(controlPointCount);
    for (int i = 0; i < controlPointCount; ++i) {
        std::string line;
        std::getline(file, line);
        if (line.empty()) {
            --i;
            continue;
        }

        std::stringstream lineStream(line);
        std::string token;

        std::getline(lineStream, token, ',');
        controlPoints[i].x = std::stof(token);

        std::getline(lineStream, token, ',');
        controlPoints[i].y = std::stof(token);

        std::getline(lineStream, token, ',');
        controlPoints[i].z = std::stof(token);
    }

    for (const auto& indexes : patchIndexes) {
        BezierPatch patch;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                patch.controlPoints[i][j] = controlPoints[indexes[i * 4 + j]];
        patches.push_back(patch);
    }

    file.close();
    return patches;
}

void generateBezier(const std::string& controlPointsFile, int tessellationLevel, const std::string& outputFile) {
    std::vector<BezierPatch> patches = readBezierPatches(controlPointsFile);

    if (patches.empty()) {
        std::cerr << "No Bezier patches found in file: " << controlPointsFile << "\n";
        return;
    }

    std::vector<Point> vertices;
    for (const auto& patch : patches) {
        int divisions = tessellationLevel;
        float step = 1.0f / divisions;

        std::vector<std::vector<Point>> gridPoints(divisions + 1, std::vector<Point>(divisions + 1));

        for (int i = 0; i <= divisions; i++) {
            for (int j = 0; j <= divisions; j++) {
                float u = i * step;
                float v = j * step;
                Point pos = evaluateBezierPatch(patch, u, v);
                Point du = evaluateBezierPatchDu(patch, u, v);
                Point dv = evaluateBezierPatchDv(patch, u, v);

                Point normal{
                    dv.y * du.z - dv.z * du.y,
                    dv.z * du.x - dv.x * du.z,
                    dv.x * du.y - dv.y * du.x
                };
                normalize(normal);

                Point vertex{
                    pos.x, pos.y, pos.z,
                    normal.x, normal.y, normal.z,
                    u, v
                };

                gridPoints[i][j] = vertex;
            }
        }

        for (int i = 0; i < divisions; i++) {
            for (int j = 0; j < divisions; j++) {
                // T1
                vertices.push_back(gridPoints[i][j]);
                vertices.push_back(gridPoints[i][j + 1]);
                vertices.push_back(gridPoints[i + 1][j]);

                // T2
                vertices.push_back(gridPoints[i + 1][j]);
                vertices.push_back(gridPoints[i][j + 1]);
                vertices.push_back(gridPoints[i + 1][j + 1]);
            }
        }
    }

    writeVerticesToFile(vertices, outputFile);
}