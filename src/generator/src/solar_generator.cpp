#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>

#include "../include/solar_generator.hpp"
#include "structs.h"

static const char* kAsteroidBeltTexture = "asteroid_belt.jpg";
static const char* kCometTexture = "asteroid.jpg";

std::vector<Point> generateOrbit(float radius, int numPoints) {
	std::vector<Point> points;

	for (int i = 0; i < numPoints; i++) {
		float angle = 2.0f * M_PI * i / numPoints;

		Point p;
		p.x = radius * cos(angle);
		p.y = 0.0f;
		p.z = radius * sin(angle);

		points.push_back(p);
	}
	return points;
}

void writeOrbit(std::ofstream& file, float radius, int numPoints, float time, int indent) {
	auto points = generateOrbit(radius, numPoints);

	std::string tab(indent, '\t');

	file << tab << "<translate time=\"" << time << "\">\n";

	for (const auto& p : points) {
		file << tab << "\t<point x=\"" << p.x
			 << "\" y=\"" << p.y
			 << "\" z=\"" << p.z << "\"/>\n";
	}

	file << tab << "</translate>\n";
}

static void writeModel(std::ofstream& file, int indent, const std::string& modelFile, const char* textureFile) {
	std::string tab(indent, '\t');
	if (textureFile && *textureFile) {
		file << tab << "<model file=\"" << modelFile << "\">\n";
		file << tab << "\t<texture file=\"" << textureFile << "\" />\n";
		file << tab << "</model>\n";
		return;
	}

	file << tab << "<model file=\"" << modelFile << "\"/>\n";
}

static const char* getPlanetTexture(const std::string& name){
	if (name == "SUN") return "2k_sun.jpg";
	if (name == "MERCURY") return "2k_mercury.jpg";
	if (name == "VENUS") return "2k_venus_surface.jpg";
	if (name == "EARTH") return "2k_earth_daymap.jpg";
	if (name == "MARS") return "2k_mars.jpg";
	if (name == "JUPITER") return "2k_jupiter.jpg";
	if (name == "SATURN") return "2k_saturn.jpg";
	if (name == "URANUS") return "2k_uranus.jpg";
	if (name == "NEPTUNE") return "2k_neptune.jpg";
	return nullptr;
}

static std::string addTextureToModelLine(const std::string& line, const char* textureFile) {
	if (!textureFile || !*textureFile) {
		return line;
	}

	if (line.find("<texture") != std::string::npos) {
		return line;
	}

	const std::string modelTag = "<model";
	const std::string selfClose = "/>";
	std::string result = line;
	size_t modelPos = result.find(modelTag);
	if (modelPos == std::string::npos) {
		return line;
	}

	size_t closePos = result.find(selfClose, modelPos);
	if (closePos == std::string::npos) {
		return line;
	}

	result.replace(closePos, selfClose.size(), "><texture file=\"" + std::string(textureFile) + "\" /></model>");
	return result;
}

void writePlanet(std::ofstream& file, const std::string& name,
				 float radius, int numPoints,
				 float orbitTime, float scale,
				 float rotationTime, int indent,
				 bool textured) {

	std::string tab(indent, '\t');

	file << tab << "<!-- " << name << " -->\n";
	file << tab << "<group>\n";

	file << tab << "\t<transform>\n";

	writeOrbit(file, radius, numPoints, orbitTime, indent + 2);

	if (name == "URANUS"){
		file << tab << "\t\t<rotate time=\"" << rotationTime << "\" x=\"0.98\" y=\"0\" z=\"0\"/>\n";
	}
	else if (name == "VENUS") {
		file << tab << "\t\t<rotate time=\"" << rotationTime << "\" x=\"0\" y=\"-1\" z=\"0\"/>\n";
	}
	else {
		file << tab << "\t\t<rotate time=\"" << rotationTime << "\" x=\"0\" y=\"1\" z=\"0\"/>\n";
	}

	file << tab << "\t\t<scale x=\"" << scale << "\" y=\"" << scale << "\" z=\"" << scale << "\"/>\n";

	file << tab << "\t</transform>\n";

	file << tab << "\t<models>\n";
	if (textured) {
		writeModel(file, indent + 2, "sphere.3d", getPlanetTexture(name));
	} else {
		writeModel(file, indent + 2, "sphere.3d", nullptr);
	}
	file << tab << "\t</models>\n";

	if (name == "EARTH") {
		file << tab << "\t<!-- MOON -->\n";
		file << tab << "\t<group>\n";
		file << tab << "\t\t<transform>\n";

		file << tab << "\t\t\t<rotate time=\"30\" x=\"0\" y=\"1\" z=\"0\"/>\n";
		file << tab << "\t\t\t<translate x=\"1.7\" y=\"0\" z=\"0\"/>\n";
		file << tab << "\t\t\t<scale x=\"0.2\" y=\"0.2\" z=\"0.2\"/>\n";

		file << tab << "\t\t</transform>\n";
		file << tab << "\t\t<models>\n";
		if (textured) {
			writeModel(file, indent + 3, "sphere.3d", "2k_moon.jpg");
		} else {
			writeModel(file, indent + 3, "sphere.3d", nullptr);
		}
		file << tab << "\t\t</models>\n";
		file << tab << "\t</group>\n";
	}

	if (name == "SATURN") {
		file << tab << "\t<!-- SATURN RING -->\n";
		file << tab << "\t<group>\n";
		file << tab << "\t\t<transform>\n";
		file << tab << "\t\t\t<rotate angle=\"25\" x=\"1\" y=\"0\" z=\"0\"/>\n";
		file << tab << "\t\t\t<scale x=\"1.2\" y=\"0.2\" z=\"1.2\"/>\n";
		file << tab << "\t\t</transform>\n";
		file << tab << "\t\t<models>\n";
		if (textured) {
			writeModel(file, indent + 3, "torus.3d", "2k_saturn_ring_alpha.png");
		} else {
			writeModel(file, indent + 3, "torus.3d", nullptr);
		}
		file << tab << "\t\t</models>\n";
		file << tab << "\t</group>\n";
	}

	file << tab << "</group>\n\n";
}

void writeAsteroidBelt(std::ofstream& file, const std::string& beltPath, int indent) {

	std::string tab(indent, '\t');

	file << tab << "<!-- ASTEROID BELT -->\n";
	file << tab << "<group>\n";
	file << tab << "\t<transform>\n";

	file << tab << "\t\t<rotate time=\"50\" x=\"0\" y=\"1\" z=\"0\"/>\n";
	file << tab << "\t\t<scale x=\"22\" y=\"18\" z=\"22\"/>\n";
	
	file << tab << "\t</transform>\n";

	std::ifstream beltFile(beltPath);
	if (!beltFile.is_open()) {
		std::cerr << "Erro ao abrir belt.xml na path " << beltPath << "\n";
		return;
	}

	std::string line;
	while (std::getline(beltFile, line)) {
		file << tab << "\t" << addTextureToModelLine(line, kAsteroidBeltTexture) << "\n";
	}

	file << tab << "</group>\n\n";
}

void writeComet(std::ofstream& file, int indent) {

	std::string tab(indent, '\t');

	file << tab << "<!-- COMET -->\n";
	file << tab << "<group>\n";

	file << tab << "\t<transform>\n";

	file << tab << "\t\t<translate time=\"35\" align=\"True\">\n";

	file << tab << "\t\t\t<point x=\"85\"  y=\"-20\" z=\"60\"/>\n";
    file << tab << "\t\t\t<point x=\"115\" y=\"15\"  z=\"90\"/>\n";
    file << tab << "\t\t\t<point x=\"145\" y=\"45\"  z=\"75\"/>\n";
    file << tab << "\t\t\t<point x=\"130\" y=\"10\"  z=\"30\"/>\n";
    file << tab << "\t\t\t<point x=\"105\" y=\"-20\" z=\"15\"/>\n";
    file << tab << "\t\t\t<point x=\"90\"  y=\"-25\" z=\"35\"/>\n";
	
	file << tab << "\t\t</translate>\n";

	file << tab << "\t\t<rotate time=\"20\" x=\"0\" y=\"1\" z=\"0\"/>\n";
	file << tab << "\t\t<scale x=\"2.5\" y=\"2.5\" z=\"2.5\"/>\n";

	file << tab << "\t</transform>\n";

	file << tab << "\t<models>\n";
	writeModel(file, indent + 2, "comet.3d", kCometTexture);
	file << tab << "\t</models>\n";

	file << tab << "</group>\n\n";
}

void generateSolarSystem(const std::string& filename, const std::string& beltPath) {
	std::ofstream file(filename);

	if (!file.is_open()) {
		std::cerr << "Erro ao criar ficheiro XML\n";
		return;
	}

	file << "<world>\n";

	file << "\t<window width=\"800\" height=\"800\"/>\n";

	file << "\t<camera>\n";
	file << "\t\t<position x=\"0\" y=\"120\" z=\"300\"/>\n";
	file << "\t\t<lookAt x=\"0\" y=\"0\" z=\"0\"/>\n";
	file << "\t\t<up x=\"0\" y=\"1\" z=\"0\"/>\n";
	file << "\t\t<projection fov=\"60\" near=\"1\" far=\"1000\"/>\n";
	file << "\t</camera>\n";

	file << "\t<lights>\n";
	file << "\t\t<light type=\"point\" posX=\"0\" posY=\"0\" posZ=\"0\"/>\n";
	file << "\t\t<light type=\"directional\" dirX=\"1\" dirY=\"1\" dirZ=\"1\"/>\n";
	file << "\t</lights>\n";

	file << "\t<group>\n";

	file << "\t\t<!-- SUN -->\n";
	file << "\t\t<group>\n";
	file << "\t\t\t<transform>\n";
	file << "\t\t\t\t<rotate time=\"25\" x=\"0\" y=\"1\" z=\"0\"/>\n";
	file << "\t\t\t\t<scale x=\"18\" y=\"18\" z=\"18\"/>\n";
	file << "\t\t\t</transform>\n";
	file << "\t\t\t<models>\n";
	writeModel(file, 4, "sphere.3d", getPlanetTexture("SUN"));
	file << "\t\t\t</models>\n";
	file << "\t\t</group>\n";

	writePlanet(file, "MERCURY", 25, 16, 15, 1.8, 15, 2, true);
	writePlanet(file, "VENUS",   34, 16, 20, 2.7, 7, 2, true);
	writePlanet(file, "EARTH",   45, 16, 24, 4.0, 12, 2, true);
	writePlanet(file, "MARS",    60, 16, 30, 3.0, 12, 2, true);

	writeAsteroidBelt(file, beltPath, 2);
	writeComet(file, 2);

	writePlanet(file, "JUPITER", 120, 16, 45, 9.0, 7, 2, true);
	writePlanet(file, "SATURN",  150, 16, 60, 7.5, 7, 2, true);
	writePlanet(file, "URANUS",  180, 16, 90, 4.5, 9, 2, true);
	writePlanet(file, "NEPTUNE", 210, 16, 110, 4.5, 9, 2, true);

	file << "\t</group>\n";
	file << "</world>\n";

	file.close();

	std::cout << "Solar system XML gerado: " << filename << std::endl;
}