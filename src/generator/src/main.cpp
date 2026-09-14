#include <iostream>
#include "../include/box.hpp"
#include "../include/cone.hpp"
#include "../include/sphere.hpp"
#include "../include/plane.hpp"
#include "../include/cylinder.hpp"
#include "../include/torus.hpp"
#include "../include/bezier.hpp"
#include "../include/comet_patch.hpp"
#include "../include/solar_generator.hpp"


int main(int argc, char** argv){

    std::string type = argv[1];

    if(type == "plane"){
        float size = std::stof(argv[2]);
        int divisions = std::stoi(argv[3]);
        std::string filename = argv[4];
        generatePlane(size, divisions, filename);
    } else if(type == "box"){
        float size = std::stof(argv[2]);
        int divisions = std::stoi(argv[3]);
        std::string filename = argv[4];
        generateBox(size, divisions, filename);
    } else if(type == "cone"){
        float radius = std::stof(argv[2]);
        float height = std::stof(argv[3]);
        int slices = std::stoi(argv[4]);
        int stacks = std::stoi(argv[5]);
        std::string filename = argv[6];
        generateCone(radius, height, slices, stacks, filename);    
    } else if(type == "cylinder"){
        float radius = std::stof(argv[2]);
        float height = std::stof(argv[3]);
        int slices = std::stoi(argv[4]);
        std::string filename = argv[5];
        generateCylinder(radius, height, slices, filename);
    } else if(type == "sphere"){
        float radius = std::stof(argv[2]);
        int slices = std::stoi(argv[3]);
        int stacks = std::stoi(argv[4]);
        std::string filename = argv[5];
        generateSphere(radius, slices, stacks, filename);
    }
    else if(type == "torus"){
        float majorRadius = std::stof(argv[2]);
        float minorRadius = std::stof(argv[3]);
        int slices = std::stoi(argv[4]);
        int stacks = std::stoi(argv[5]);
        std::string filename = argv[6];
        generateTorus(majorRadius, minorRadius, slices, stacks, filename);
    }
    else if(type == "bezier"){
        std::string controlPointsFile = argv[2];
        int tessellationLevel = std::stoi(argv[3]);
        std::string outputFile = argv[4];
        generateBezier(controlPointsFile, tessellationLevel, outputFile);
    }
    else if(type == "comet"){
        std::string filename = argv[2];
        generateComet(filename);
    }
    else if(type == "solar"){
        std::string filename = argv[2];
        std::string beltPath = argv[3];
        generateSolarSystem(filename, beltPath);
    }

    return 0;
}