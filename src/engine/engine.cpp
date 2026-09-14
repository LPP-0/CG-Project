#include "include/engine.h"
#include "include/parser.h"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <IL/il.h>


struct ModelData {
    std::vector<float> vertexData;
};

const std::vector<Point>* currentControlPoints = nullptr;


ILstring toIlString(const std::string& path, std::wstring& wideStorage) {
#if defined(_UNICODE)
    wideStorage = std::filesystem::path(path).wstring();
    return const_cast<wchar_t*>(wideStorage.c_str());
#else
    (void)wideStorage;
    return const_cast<char*>(path.c_str());
#endif
}

void multMatrixVector(const float m[4][4], const float* v, float* res) {
    for (int j = 0; j < 4; ++j) {
        res[j] = 0.0f;
        for (int k = 0; k < 4; ++k) {
            res[j] += v[k] * m[j][k];
        }
    }
}

void cross(const float* a, const float* b, float* res) {
    res[0] = a[1] * b[2] - a[2] * b[1];
    res[1] = a[2] * b[0] - a[0] * b[2];
    res[2] = a[0] * b[1] - a[1] * b[0];
}

void normalize(float* a) {
    float len = std::sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
    if (len > 0.0f) {
        a[0] /= len;
        a[1] /= len;
        a[2] /= len;
    }
}

void buildRotMatrix(const float* x, const float* y, const float* z, float* m) {
    m[0] = x[0]; m[1] = x[1]; m[2] = x[2]; m[3] = 0.0f;
    m[4] = y[0]; m[5] = y[1]; m[6] = y[2]; m[7] = 0.0f;
    m[8] = z[0]; m[9] = z[1]; m[10] = z[2]; m[11] = 0.0f;
    m[12] = 0.0f; m[13] = 0.0f; m[14] = 0.0f; m[15] = 1.0f;
}

void getCatmullRomPoint(float t, const Point& p0, const Point& p1, const Point& p2, const Point& p3, float* pos, float* deriv) {
    const float m[4][4] = {
        {-0.5f, 1.5f, -1.5f, 0.5f},
        {1.0f, -2.5f, 2.0f, -0.5f},
        {-0.5f, 0.0f, 0.5f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f}
    };

   // A curvatura de cada eixo depende apenas das coordenadas desse mesmo eixo nos 4 pontos.
   // Listas de coordenadas para processar um eixo de cada vez.
    const float px[4] = {p0.x, p1.x, p2.x, p3.x};
    const float py[4] = {p0.y, p1.y, p2.y, p3.y};
    const float pz[4] = {p0.z, p1.z, p2.z, p3.z};

    // Multiplicamos a matriz de base pelos pontos para obter os coeficientes (a, b, c, d)
    // da equação cúbica: P(t) = at³ + bt² + ct + d
    float ax[4], ay[4], az[4];
    multMatrixVector(m, px, ax);
    multMatrixVector(m, py, ay);
    multMatrixVector(m, pz, az);


    const float t2 = t * t;
    const float t3 = t2 * t;

    pos[0] = t3 * ax[0] + t2 * ax[1] + t * ax[2] + ax[3];
    pos[1] = t3 * ay[0] + t2 * ay[1] + t * ay[2] + ay[3];
    pos[2] = t3 * az[0] + t2 * az[1] + t * az[2] + az[3];

    deriv[0] = 3.0f * t2 * ax[0] + 2.0f * t * ax[1] + ax[2];
    deriv[1] = 3.0f * t2 * ay[0] + 2.0f * t * ay[1] + ay[2];
    deriv[2] = 3.0f * t2 * az[0] + 2.0f * t * az[1] + az[2];
}

void getGlobalCatmullRomPoint(float gt, float* pos, float* deriv) {
    if (!currentControlPoints || currentControlPoints->size() < 4) {
        pos[0] = pos[1] = pos[2] = 0.0f;
        deriv[0] = deriv[1] = deriv[2] = 0.0f;
        return;
    }

    const auto& points = *currentControlPoints;
    const int pointCount = static_cast<int>(points.size());
    const float t = gt * pointCount;
    const int index = static_cast<int>(std::floor(t));
    const float localT = t - index;

    // Indices dos 4 pontos de controlo que influenciam a posição atual.
    const int indices[4] = {
        (index + pointCount - 1) % pointCount,
        index % pointCount,
        (index + 1) % pointCount,
        (index + 2) % pointCount
    };

    getCatmullRomPoint(
        localT,
        points[indices[0]],
        points[indices[1]],
        points[indices[2]],
        points[indices[3]],
        pos,
        deriv
    );
}

bool loadModelData(const std::string& filePath, ModelData& data) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir " << filePath << std::endl;
        return false;
    }

    std::string headerLine;
    if (!std::getline(file, headerLine)) {
        return false;
    }

    std::istringstream headerStream(headerLine);
    unsigned int vertexCount = 0;

    if (!(headerStream >> vertexCount)) {
        return false;
    }

    data.vertexData.reserve(vertexCount * 8);
    for (unsigned int i = 0; i < vertexCount; ++i) {
        std::string vertexLine;
        if (!std::getline(file, vertexLine)) {
            return false;
        }

        std::istringstream vertexStream(vertexLine);
        std::vector<float> values;
        float value = 0.0f;

        while (vertexStream >> value) {
            values.push_back(value);
        }

        if (values.size() != 8) {
            return false;
        }

        data.vertexData.insert(data.vertexData.end(), values.begin(), values.end());
    }

    return true;
}

GLuint loadTexture(const std::string& path) {
    ILuint imageID;
    ilGenImages(1, &imageID);
    ilBindImage(imageID);

    std::wstring widePath;
    ILstring ilPath = toIlString(path, widePath);
    if (!ilLoadImage(ilPath)) {
        ilDeleteImages(1, &imageID);
        return 0;
    }

    if (!ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE)) {
        ilDeleteImages(1, &imageID);
        return 0;
    }

    const int width = ilGetInteger(IL_IMAGE_WIDTH);
    const int height = ilGetInteger(IL_IMAGE_HEIGHT);
    unsigned char* data = ilGetData();

    GLuint texId = 0;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
   
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    ilDeleteImages(1, &imageID);

    return texId;
}

void generateCurveVBO(Transformation& t) {
    if (t.controlPoints.size() < 4) return;

    std::vector<float> vertices;
    int segments = 100; 
    t.curveVertexCount = segments;

    const std::vector<Point>* backup = currentControlPoints;
    currentControlPoints = &t.controlPoints;

    for (int i = 0; i < segments; i++) {
        float gt = (float)i / (float)segments;
        float pos[3], deriv[3];
        getGlobalCatmullRomPoint(gt, pos, deriv);
        vertices.push_back(pos[0]);
        vertices.push_back(pos[1]);
        vertices.push_back(pos[2]);
    }
    currentControlPoints = backup;

    // Criar o VBO específico para esta curva
    glGenBuffers(1, &t.curveVbo);
    glBindBuffer(GL_ARRAY_BUFFER, t.curveVbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void initGroupBuffers(Group& g) {

    for (auto& t : g.transforms) {
        if (t.type == "translate" && !t.controlPoints.empty()) {
            generateCurveVBO(t);
        }
    }

    for (auto& model : g.models) {
        ModelData data;
        if (!loadModelData(model.filePath, data)) {
            std::cerr << "Falha ao carregar: " << model.filePath << std::endl;
            continue;
        }

        model.vertexCount = static_cast<unsigned int>(data.vertexData.size() / 8);
        std::cout << "Modelo carregado: " << model.filePath << " vertices=" << model.vertexCount << std::endl;

        glGenBuffers(1, model.vertices);
        glBindBuffer(GL_ARRAY_BUFFER, model.vertices[0]);
        glBufferData(
            GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(data.vertexData.size() * sizeof(float)),
            data.vertexData.data(),
            GL_STATIC_DRAW
        );

        // Carregar textura associada
        if (!model.textureFile.empty()) {
            model.texture = loadTexture(model.textureFile);
        }

    }

    for (auto& child : g.children) {
        initGroupBuffers(child);
    }

}


void initSceneBuffers(Scene& scene) {
    initGroupBuffers(scene.root);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void renderCurve(const Transformation& t) {
    if (t.curveVbo == 0 || t.curveVertexCount == 0) return;

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);

    glBindBuffer(GL_ARRAY_BUFFER, t.curveVbo);
    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, 0);
    glDrawArrays(GL_LINE_LOOP, 0, t.curveVertexCount);

    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glPopAttrib();
}

void renderGroup(const Group& g) {
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);

    // 1. Aplicar transformações na ordem do XML
    for (const auto& t : g.transforms) {
        if (t.type == "translate") {
            if (!t.controlPoints.empty() && t.time > 0.0f) {
                renderCurve(t);

                float pos[3] = {0.0f, 0.0f, 0.0f};
                float deriv[3] = {0.0f, 0.0f, 0.0f};
                const float elapsedSeconds = static_cast<float>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
                const float curveT = std::fmod(elapsedSeconds, t.time) / t.time;

                currentControlPoints = &t.controlPoints;
                getGlobalCatmullRomPoint(curveT, pos, deriv);

                glTranslatef(pos[0], pos[1], pos[2]);

                if (t.align) {
                    float x[3] = {deriv[0], deriv[1], deriv[2]};
                    normalize(x);

                    float y[3] = {0.0f, 1.0f, 0.0f};
                    float z[3];
                    cross(x, y, z);
                    normalize(z);

                    cross(z, x, y);
                    normalize(y);

                    float rotMatrix[16];
                    buildRotMatrix(x, y, z, rotMatrix);
                    glMultMatrixf(rotMatrix);
                }
            } else {
                glTranslatef(t.x, t.y, t.z);
            }
        } else if (t.type == "rotate") {
            float angle = t.angle;
            if (t.time > 0.0f) {
                const float elapsedSeconds = static_cast<float>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
                angle = std::fmod(elapsedSeconds, t.time) * (360.0f / t.time);
            }
            glRotatef(angle, t.x, t.y, t.z);
        } else if (t.type == "scale") {
            glScalef(t.x, t.y, t.z);
        }
    }

    // 2. Desenhar os modelos do grupo atual
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    for (const auto& model : g.models) {
        if (model.vertices[0] == 0 || model.vertexCount == 0) {
            continue;
        }

        glBindBuffer(GL_ARRAY_BUFFER, model.vertices[0]);
        glVertexPointer(3, GL_FLOAT, 8 * sizeof(float), reinterpret_cast<const void*>(0));
        glNormalPointer(GL_FLOAT, 8 * sizeof(float), reinterpret_cast<const void*>(3 * sizeof(float)));
        glTexCoordPointer(2, GL_FLOAT, 8 * sizeof(float), reinterpret_cast<const void*>(6 * sizeof(float)));

        if (model.texture != 0) {
            glBindTexture(GL_TEXTURE_2D, model.texture);
        } else {     
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // Aplicar material do modelo
        glMaterialfv(GL_FRONT, GL_AMBIENT, model.material.ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, model.material.diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, model.material.specular);
        glMaterialfv(GL_FRONT, GL_EMISSION, model.material.emissive);
        glMaterialf(GL_FRONT, GL_SHININESS, model.material.shininess);

        glDrawArrays(GL_TRIANGLES, 0, model.vertexCount);
    }
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);

    // 3. Chamar recursivamente para os filhos
    for (const auto& child : g.children) {
        renderGroup(child);
    }

    glPopMatrix();
}

void setupLighting(const Scene& scene) {
    
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_RESCALE_NORMAL);

    float globalAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    // Configurar cada luz
    for (size_t i = 0; i < scene.lights.size() && i < 8; ++i) {
        const Light& light = scene.lights[i];
        GLenum lightId = GL_LIGHT0 + i;

        glEnable(lightId);

        float lightAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
        float lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
        float lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};

        glLightfv(lightId, GL_AMBIENT, lightAmbient);
        glLightfv(lightId, GL_DIFFUSE, lightDiffuse);
        glLightfv(lightId, GL_SPECULAR, lightSpecular);

        if (light.type == "point") {
            // Point light: posição com w=1.0
            float position[] = {light.position[0], light.position[1], light.position[2], 1.0f};
            glLightfv(lightId, GL_POSITION, position);
        } else if (light.type == "directional") {
            // Directional light: posição com w=0.0 (infinita)
            float position[] = {light.direction[0], light.direction[1], light.direction[2], 0.0f};
            glLightfv(lightId, GL_POSITION, position);
        } else if (light.type == "spotlight") {
            // Spotlight: posição com w=1.0, direção e cutoff
            float position[] = {light.position[0], light.position[1], light.position[2], 1.0f};
            glLightfv(lightId, GL_POSITION, position);
            glLightfv(lightId, GL_SPOT_DIRECTION, light.direction);
            glLightf(lightId, GL_SPOT_CUTOFF, light.cutoff);
            glLightf(lightId, GL_SPOT_EXPONENT, 2.0f); 
        }
    }

    for (size_t i = scene.lights.size(); i < 8; ++i) {
        glDisable(GL_LIGHT0 + i);
    }
}

void updateLightPositions(const Scene& scene) {
    for (size_t i = 0; i < scene.lights.size() && i < 8; ++i) {
        const Light& light = scene.lights[i];
        GLenum lightId = GL_LIGHT0 + static_cast<GLenum>(i);

        if (light.type == "point") {
            float position[] = {light.position[0], light.position[1], light.position[2], 1.0f};
            glLightfv(lightId, GL_POSITION, position);
        } else if (light.type == "directional") {
            float position[] = {light.direction[0], light.direction[1], light.direction[2], 0.0f};
            glLightfv(lightId, GL_POSITION, position);
        } else if (light.type == "spotlight") {
            float position[] = {light.position[0], light.position[1], light.position[2], 1.0f};
            glLightfv(lightId, GL_POSITION, position);
            glLightfv(lightId, GL_SPOT_DIRECTION, light.direction);
            glLightf(lightId, GL_SPOT_CUTOFF, light.cutoff);
        }
    }
    for (size_t i = scene.lights.size(); i < 8; ++i) {
        glDisable(GL_LIGHT0 + static_cast<GLenum>(i));
    }
}

void drawAxes() {
    glBegin(GL_LINES);
    
    // Eixo X - Vermelho
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-100.0f, 0.0f, 0.0f);
    glVertex3f(100.0f, 0.0f, 0.0f);

    // Eixo Y - Verde
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -100.0f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);

    // Eixo Z - Azul
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -100.0f);
    glVertex3f(0.0f, 0.0f, 100.0f);
    
    glEnd();
}

void updateCamera(float& alpha, float& beta, float& radius, Camera& camera) {
    if (radius < 1.0f) radius = 1.0f;
    
    if (beta > 1.5f) beta = 1.5f;
    if (beta < -1.5f) beta = -1.5f;

    camera.position.x = radius * cos(beta) * sin(alpha);
    camera.position.y = radius * sin(beta);
    camera.position.z = radius * cos(beta) * cos(alpha);
}

void syncCamera(float& alpha, float& beta, float& radius, Camera& camera) {
    float x = camera.position.x;
    float y = camera.position.y;
    float z = camera.position.z;

    radius = sqrt(x*x + y*y + z*z);
    alpha = atan2(x, z);
    beta = asin(y / radius);
}

