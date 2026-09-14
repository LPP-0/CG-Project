set(GENERATOR_SOURCES 
    src/main.cpp 
    src/plane.cpp 
    src/box.cpp 
    src/sphere.cpp 
    src/cone.cpp 
    src/cylinder.cpp 
    src/torus.cpp
    src/bezier.cpp
    src/comet_patch.cpp
    src/solar_generator.cpp
    utils/writeVertices.cpp
)

add_executable(generator ${GENERATOR_SOURCES})

target_include_directories(generator PRIVATE include utils ../common)