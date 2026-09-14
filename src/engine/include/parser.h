#ifndef PARSER_H
#define PARSER_H

#include "structs.h"
#include <string>
#include <vector>

// Forward declaration for tinyxml2
namespace tinyxml2 {
    class XMLElement;
}

void parseXML(const std::string& path, Scene& scene);
void parseGroup(tinyxml2::XMLElement* groupElement, Group& currentGroup);

#endif