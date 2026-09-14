#ifndef COMET_HPP
#define COMET_HPP

#include <string>
#include "structs.h"


Point spherePoint(float u, float v);
float wrappedVDistance(float v, float centerV);
float rockyNoise(float u, float v);
float calcCrater(float u, float v, float cu, float cv, float radius, float depth, float rimHeight);
Point deform(Point p, float u, float v);


void generateComet(const std::string& outputFile);

#endif
