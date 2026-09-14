#include "../include/box.hpp"
#include "structs.h"
#include "../utils/writeVertices.hpp"

void generateBox(float size, int divisions, const std::string& filename) {
    std::vector<Point> vertices;
    float half = size / 2.0f;
    float step = size / (float)divisions;

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            // Definição dos limites da célula atual
            float a = -half + j * step;       
            float b = -half + (j + 1) * step; 
            float c = -half + i * step;       
            float d = -half + (i + 1) * step; 

            // Limites de textura
            float u1 = (float)j / divisions;
            float u2 = (float)(j + 1) / divisions;
            float v1 = (float)i / divisions;
            float v2 = (float)(i + 1) / divisions;

            // Face Superior (Y fixa em half)
            float ny_up = 1.0f;
            vertices.push_back({b, half, c, 0, ny_up, 0, u2, v1}); 
            vertices.push_back({a, half, c, 0, ny_up, 0, u1, v1}); 
            vertices.push_back({a, half, d, 0, ny_up, 0, u1, v2});
            
            vertices.push_back({b, half, c, 0, ny_up, 0, u2, v1});           
            vertices.push_back({a, half, d, 0, ny_up, 0, u1, v2}); 
            vertices.push_back({b, half, d, 0, ny_up, 0, u2, v2});
    
            // Face Inferior (Y fixa em -half)
            float ny_down = -1.0f;
            vertices.push_back({a, -half, d, 0, ny_down, 0, u1, v2}); 
            vertices.push_back({a, -half, c, 0, ny_down, 0, u1, v1}); 
            vertices.push_back({b, -half, c, 0, ny_down, 0, u2, v1});
            
            vertices.push_back({a, -half, d, 0, ny_down, 0, u1, v2}); 
            vertices.push_back({b, -half, c, 0, ny_down, 0, u2, v1}); 
            vertices.push_back({b, -half, d, 0, ny_down, 0, u2, v2});

            // Face Frontal (Z fixa em half)
            vertices.push_back({b, d, half, 0, 0, 1.0f, u2, v2}); 
            vertices.push_back({a, d, half, 0, 0, 1.0f, u1, v2}); 
            vertices.push_back({a, c, half, 0, 0, 1.0f, u1, v1});
            
            vertices.push_back({b, d, half, 0, 0, 1.0f, u2, v2}); 
            vertices.push_back({a, c, half, 0, 0, 1.0f, u1, v1}); 
            vertices.push_back({b, c, half, 0, 0, 1.0f, u2, v1});

            // Face Posterior (Z fixa em -half)
            vertices.push_back({a, c, -half, 0, 0, -1.0f, u1, v1}); 
            vertices.push_back({a, d, -half, 0, 0, -1.0f, u1, v2}); 
            vertices.push_back({b, d, -half, 0, 0, -1.0f, u2, v2});
            
            vertices.push_back({a, c, -half, 0, 0, -1.0f, u1, v1}); 
            vertices.push_back({b, d, -half, 0, 0, -1.0f, u2, v2}); 
            vertices.push_back({b, c, -half, 0, 0, -1.0f, u2, v1});

            // Face Direita (X fixa em half)
            vertices.push_back({half, d, a, 1.0f, 0, 0, u2, v2});
            vertices.push_back({half, d, b, 1.0f, 0, 0, u2, v1});
            vertices.push_back({half, c, b, 1.0f, 0, 0, u1, v1});

            vertices.push_back({half, d, a, 1.0f, 0, 0, u2, v2});
            vertices.push_back({half, c, b, 1.0f, 0, 0, u1, v1});
            vertices.push_back({half, c, a, 1.0f, 0, 0, u1, v2});

            // Face Esquerda (X fixa em -half)
            vertices.push_back({-half, d, b, -1.0f, 0, 0, u2, v2}); 
            vertices.push_back({-half, d, a, -1.0f, 0, 0, u2, v1}); 
            vertices.push_back({-half, c, a, -1.0f, 0, 0, u1, v1});
            
            vertices.push_back({-half, d, b, -1.0f, 0, 0, u2, v2}); 
            vertices.push_back({-half, c, a, -1.0f, 0, 0, u1, v1}); 
            vertices.push_back({-half, c, b, -1.0f, 0, 0, u1, v2});
        }
    }
    writeVerticesToFile(vertices, filename);
}