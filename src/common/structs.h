#ifndef STRUCTS_H
#define STRUCTS_H

typedef unsigned int GLuint;

#include <vector>
#include <string>

struct Point {
    float x, y, z;
    float nx, ny, nz; // Normais para iluminação
    float u, v;       // Coordenadas de textura
};

struct Transformation {
    std::string type; // "translate", "rotate", "scale"
    float x, y, z, angle;
    float time = 0.0f;
    bool align = false;
    std::vector<Point> controlPoints;

    GLuint curveVbo = 0;       // ID do Buffer no GPU
    int curveVertexCount = 0;  // Número de pontos a desenhar
};

struct Material {
    float ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};     // RGBA
    float diffuse[4] = {0.784f, 0.784f, 0.784f, 1.0f}; // RGBA (200,200,200 default)
    float specular[4] = {1.0f, 1.0f, 1.0f, 1.0f};     // RGBA
    float emissive[4] = {0.0f, 0.0f, 0.0f, 1.0f};    // RGBA
    float shininess = 32.0f;
};

struct Light {
    std::string type; // "point", "directional", "spotlight"
    float position[3] = {0.0f, 0.0f, 0.0f};
    float direction[3] = {0.0f, 0.0f, -1.0f};
    float cutoff = 45.0f;
};

struct Model {
    std::string filePath;
    GLuint vertices[1] = {0};
    unsigned int vertexCount = 0;
    std::string textureFile;
    GLuint texture = 0;
    Material material;
};

struct Group {
    std::vector<Transformation> transforms;
    std::vector<Model> models;
    std::vector<Group> children;
};

struct Camera {
    Point position;
    Point lookAt;
    Point up;
    float fov, nearPlane, farPlane;
};

struct Window {
    int width, height;
};

struct Scene {
    Window window;
    Camera camera;
    Group root; // O nó principal que contém toda a hierarquia
    std::vector<Light> lights;
};

#endif