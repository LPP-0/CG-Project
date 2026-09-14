cmake_minimum_required(VERSION 3.10)
project(CG_Phase4)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

file(COPY ${CMAKE_SOURCE_DIR}/scenes/ DESTINATION ${CMAKE_BINARY_DIR}/scenes/ FILES_MATCHING PATTERN "*.xml")

add_subdirectory(src/engine)
add_subdirectory(src/generator)