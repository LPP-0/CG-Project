#include "writeVertices.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "structs.h"

void writeVerticesToFile(const std::vector<Point>& vertices, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o ficheiro: " << filename << "\n";
        return;
    }

    file << vertices.size() << "\n";


    for(const auto& vertex : vertices) {
        file << vertex.x << " " << vertex.y << " " << vertex.z << " ";
        file << vertex.nx << " " << vertex.ny << " " << vertex.nz << " ";
        file << vertex.u << " " << vertex.v << "\n";
    }

    file.close();
}