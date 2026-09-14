#include "include/parser.h"
#include <iostream>
#include <string>
#include "tinyxml2.h"


static inline float byteToFloat(int value) {
    return static_cast<float>(value) / 255.0f;
}


static void parseColorComponent(tinyxml2::XMLElement* colorElem, const char* componentName, float* out) {
    out[0] = out[1] = out[2] = 0.0f;
    out[3] = 1.0f;
    
    if (!colorElem) return;
    
    tinyxml2::XMLElement* comp = colorElem->FirstChildElement(componentName);
    if (!comp) return;
    
    int r = 0, g = 0, b = 0;

    if (comp->QueryIntAttribute("r", &r) != tinyxml2::XML_SUCCESS) comp->QueryIntAttribute("R", &r);
    if (comp->QueryIntAttribute("g", &g) != tinyxml2::XML_SUCCESS) comp->QueryIntAttribute("G", &g);
    if (comp->QueryIntAttribute("b", &b) != tinyxml2::XML_SUCCESS) comp->QueryIntAttribute("B", &b);
    
    out[0] = byteToFloat(r);
    out[1] = byteToFloat(g);
    out[2] = byteToFloat(b);
    out[3] = 1.0f;
}


static void parseMaterial(tinyxml2::XMLElement* modelElem, Material& material) {
    
    tinyxml2::XMLElement* colorElem = modelElem->FirstChildElement("color");
    if (!colorElem) return;
    
  
    parseColorComponent(colorElem, "diffuse", material.diffuse);
    parseColorComponent(colorElem, "ambient", material.ambient);
    parseColorComponent(colorElem, "specular", material.specular);
    parseColorComponent(colorElem, "emissive", material.emissive);
    
    tinyxml2::XMLElement* shinElem = colorElem->FirstChildElement("shininess");
    if (shinElem) {
        shinElem->QueryFloatAttribute("value", &material.shininess);
    } else {
        colorElem->QueryFloatAttribute("shininess", &material.shininess);
    }
}

static void parseLights(tinyxml2::XMLElement* lightsElem, std::vector<Light>& lights) {
    if (!lightsElem) return;

    auto queryFloatAttr = [](tinyxml2::XMLElement* elem, const char* primary, const char* fallback, float* out) {
        if (elem->QueryFloatAttribute(primary, out) != tinyxml2::XML_SUCCESS && fallback) {
            elem->QueryFloatAttribute(fallback, out);
        }
    };
    
    for (tinyxml2::XMLElement* light = lightsElem->FirstChildElement("light"); light; light = light->NextSiblingElement("light")) {
        Light l;
        
        const char* type = light->Attribute("type");
        if (type) {
            l.type = type;
            if (l.type == "spot") {
                l.type = "spotlight";
            }
        } else {
            l.type = "point"; 
        }
        
        if (l.type == "point") {
            queryFloatAttr(light, "posX", "posx", &l.position[0]);
            queryFloatAttr(light, "posY", "posy", &l.position[1]);
            queryFloatAttr(light, "posZ", "posz", &l.position[2]);

        } else if (l.type == "directional") {
            queryFloatAttr(light, "dirX", "dirx", &l.direction[0]);
            queryFloatAttr(light, "dirY", "diry", &l.direction[1]);
            queryFloatAttr(light, "dirZ", "dirz", &l.direction[2]);

        } else if (l.type == "spotlight") {
            queryFloatAttr(light, "posX", "posx", &l.position[0]);
            queryFloatAttr(light, "posY", "posy", &l.position[1]);
            queryFloatAttr(light, "posZ", "posz", &l.position[2]);
            queryFloatAttr(light, "dirX", "dirx", &l.direction[0]);
            queryFloatAttr(light, "dirY", "diry", &l.direction[1]);
            queryFloatAttr(light, "dirZ", "dirz", &l.direction[2]);
            light->QueryFloatAttribute("cutoff", &l.cutoff);
        }
        
        lights.push_back(l);
    }
}


void parseGroup(tinyxml2::XMLElement* groupElement, Group& currentGroup) {
    if (!groupElement) return;

    // 1. Transformações
    tinyxml2::XMLElement* transElem = groupElement->FirstChildElement("transform");
    if (transElem) {
        for (tinyxml2::XMLElement* t = transElem->FirstChildElement(); t; t = t->NextSiblingElement()) {
            Transformation trans;
            trans.type = t->Value();
            trans.x = trans.y = trans.z = (trans.type == "scale" ? 1.0f : 0.0f);
            trans.angle = 0.0f;
            trans.time = 0.0f;
            trans.align = false;

            t->QueryFloatAttribute("x", &trans.x);
            t->QueryFloatAttribute("y", &trans.y);
            t->QueryFloatAttribute("z", &trans.z);
            t->QueryFloatAttribute("time", &trans.time);

            if (trans.type == "rotate") {
                t->QueryFloatAttribute("angle", &trans.angle);
            }

            if (trans.type == "translate") {
                t->QueryBoolAttribute("align", &trans.align);

                for (tinyxml2::XMLElement* pointElem = t->FirstChildElement("point"); pointElem; pointElem = pointElem->NextSiblingElement("point")) {
                    Point p{0.0f, 0.0f, 0.0f};
                    pointElem->QueryFloatAttribute("x", &p.x);
                    pointElem->QueryFloatAttribute("y", &p.y);
                    pointElem->QueryFloatAttribute("z", &p.z);
                    trans.controlPoints.push_back(p);
                }
            }
            
            currentGroup.transforms.push_back(trans);
        }
    }

    // 2. Modelos
    tinyxml2::XMLElement* modelsElem = groupElement->FirstChildElement("models");
    if (modelsElem) {
        for (tinyxml2::XMLElement* m = modelsElem->FirstChildElement("model"); m; m = m->NextSiblingElement("model")) {
            const char* file = m->Attribute("file");
            if (file) {
                std::string fullPath = file;

                if (fullPath.find("models/") != 0)
                    fullPath = "models/" + fullPath;

                Model model;
                model.filePath = fullPath;
                
                // Verificar se existe elemento filho <texture>
                tinyxml2::XMLElement* texElem = m->FirstChildElement("texture");
                if (texElem) {
                    const char* texFile = texElem->Attribute("file");
                    if (texFile) {
                        std::string texPath = texFile;
                        if (texPath.find("textures/") != 0)
                            texPath = "textures/" + texPath;
                        model.textureFile = texPath;
                    }
                } else {
                    // Fallback: tentar atributo direto (compatibilidade)
                    const char* tex = m->Attribute("texture");
                    if (tex) {
                        std::string texPath = tex;
                        if (texPath.find("textures/") != 0)
                            texPath = "textures/" + texPath;
                        model.textureFile = texPath;
                    }
                }
                
                // Parser material 
                parseMaterial(m, model.material);
                
                currentGroup.models.push_back(model);
            }
        }
    }

    // 3. Sub-grupos (Recursividade)
    for (tinyxml2::XMLElement* sub = groupElement->FirstChildElement("group"); sub; sub = sub->NextSiblingElement("group")) {
        Group child;
        parseGroup(sub, child);
        currentGroup.children.push_back(child);
    }
}


void parseXML(const std::string& path, Scene& scene) {
    std::cout << "Parsing XML file: " << path << std::endl;

    tinyxml2::XMLDocument doc;
    if(doc.LoadFile(path.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Error loading XML file: " << path << std::endl;
        return;
    }

    tinyxml2::XMLElement* world = doc.FirstChildElement("world");
    if(!world){
        std::cerr << "No <world> element found in XML." << std::endl;
        return;
    }

    // --- WINDOW ---
    tinyxml2::XMLElement* windowElement = world->FirstChildElement("window");
    if(windowElement){
        
        windowElement->QueryIntAttribute("width", &scene.window.width);
        windowElement->QueryIntAttribute("height", &scene.window.height);
        
        std::cout << "Window size: " << scene.window.width << "x" << scene.window.height << std::endl;
    }

    // --- CAMERA ---
    tinyxml2::XMLElement* cameraElement = world->FirstChildElement("camera");
    if(cameraElement){
        tinyxml2::XMLElement* position = cameraElement->FirstChildElement("position");
        tinyxml2::XMLElement* lookAt = cameraElement->FirstChildElement("lookAt");
        tinyxml2::XMLElement* up = cameraElement->FirstChildElement("up");
        tinyxml2::XMLElement* projection = cameraElement->FirstChildElement("projection");

        if(position){
            position->QueryFloatAttribute("x", &scene.camera.position.x);
            position->QueryFloatAttribute("y", &scene.camera.position.y);
            position->QueryFloatAttribute("z", &scene.camera.position.z);
        }

        if(lookAt){
            lookAt->QueryFloatAttribute("x", &scene.camera.lookAt.x);
            lookAt->QueryFloatAttribute("y", &scene.camera.lookAt.y);
            lookAt->QueryFloatAttribute("z", &scene.camera.lookAt.z);
        }

        if(up){
            up->QueryFloatAttribute("x", &scene.camera.up.x);
            up->QueryFloatAttribute("y", &scene.camera.up.y);
            up->QueryFloatAttribute("z", &scene.camera.up.z);
        }

        if(projection){
            projection->QueryFloatAttribute("fov", &scene.camera.fov);
            projection->QueryFloatAttribute("near", &scene.camera.nearPlane);
            projection->QueryFloatAttribute("far", &scene.camera.farPlane);
        }

        std::cout << "Camera loaded successfully." << std::endl;
    } else {
        std::cerr << "No <camera> element found in XML." << std::endl;
    }
    

    // --- LIGHTS ---
    tinyxml2::XMLElement* lightsElement = world->FirstChildElement("lights");
    if (lightsElement) {
        parseLights(lightsElement, scene.lights);
        std::cout << "Loaded " << scene.lights.size() << " light(s)." << std::endl;
    }

    scene.root.children.clear();
    for (auto g = world->FirstChildElement("group"); g; g = g->NextSiblingElement("group")) {
        Group child;
        parseGroup(g, child);
        scene.root.children.push_back(child);
    }

    std::cout << "Finished parsing XML." << std::endl;
}