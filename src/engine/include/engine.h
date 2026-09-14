#ifndef ENGINE_H
#define ENGINE_H
#include "structs.h"

void renderGroup(const Group& g);
void initSceneBuffers(Scene& scene);
void drawAxes();
void setupLighting(const Scene& scene);
void updateLightPositions(const Scene& scene);
void updateCamera(float& alpha, float& beta, float& radius, Camera& cam);
void syncCamera(float& alpha, float& beta, float& radius, Camera& cam);

#endif