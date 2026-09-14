find_package(OpenGL REQUIRED)

# =========================
# Sources
# =========================

set(ENGINE_SOURCES 
    main.cpp 
    parser.cpp 
    engine.cpp 
    ../../external/tinyxml2.cpp
)

add_executable(engine ${ENGINE_SOURCES})

target_include_directories(engine PRIVATE 
    include 
    ../common 
    ../../external 
)

# =========================
# Asset copy
# =========================

add_custom_command(TARGET engine POST_BUILD

    # ---- CLEAN OLD FOLDERS ----
    COMMAND ${CMAKE_COMMAND} -E remove_directory $<TARGET_FILE_DIR:engine>/scenes
    COMMAND ${CMAKE_COMMAND} -E remove_directory $<TARGET_FILE_DIR:engine>/models
    COMMAND ${CMAKE_COMMAND} -E remove_directory $<TARGET_FILE_DIR:engine>/patches
    COMMAND ${CMAKE_COMMAND} -E remove_directory $<TARGET_FILE_DIR:engine>/textures

    # ---- RECREATE STRUCTURE ----
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:engine>/scenes
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:engine>/models
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:engine>/patches
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:engine>/textures

    # ---- COPY FRESH DATA ----
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/scenes
        $<TARGET_FILE_DIR:engine>/scenes

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/models
        $<TARGET_FILE_DIR:engine>/models

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/patches
        $<TARGET_FILE_DIR:engine>/patches

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/textures
        $<TARGET_FILE_DIR:engine>/textures
)

# =========================
# WINDOWS (Toolkits setup)
# =========================

if (WIN32)

    set(TOOLKITS_FOLDER "" CACHE PATH "Path to Toolkits folder")

    # ---- GLUT ----
    if (NOT EXISTS "${TOOLKITS_FOLDER}/glut/GL/glut.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glut/glut32.lib")
        message(FATAL_ERROR "GLUT not found in toolkits folder")
    endif()

    # ---- GLEW ----
    if (NOT EXISTS "${TOOLKITS_FOLDER}/glew/GL/glew.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glew/glew32.lib")
        message(FATAL_ERROR "GLEW not found in toolkits folder")
    endif()

    # ---- DevIL ----
    if (NOT EXISTS "${TOOLKITS_FOLDER}/DevIL/IL/il.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/devil/DevIL.lib")
        message(FATAL_ERROR "DevIL not found in toolkits folder")
    endif()

    target_include_directories(engine PRIVATE
        ${TOOLKITS_FOLDER}/glut
        ${TOOLKITS_FOLDER}/glew
        ${TOOLKITS_FOLDER}/devil
    )

    target_link_libraries(engine PRIVATE
        ${OPENGL_LIBRARIES}
        ${TOOLKITS_FOLDER}/glut/glut32.lib
        ${TOOLKITS_FOLDER}/glew/glew32.lib
        ${TOOLKITS_FOLDER}/devil/DevIL.lib
    )

    add_custom_command(TARGET engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${TOOLKITS_FOLDER}/glut/glut32.dll"
            $<TARGET_FILE_DIR:engine>

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${TOOLKITS_FOLDER}/glew/glew32.dll"
            $<TARGET_FILE_DIR:engine>

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${TOOLKITS_FOLDER}/devil/devil.dll"
            $<TARGET_FILE_DIR:engine>
    )

# =========================
# MAC / LINUX
# =========================

else()

    find_package(GLUT REQUIRED)

    find_path(DEVIL_INCLUDE_DIR IL/il.h)
    find_library(DEVIL_LIBRARY NAMES IL DevIL devil)

    target_include_directories(engine PRIVATE
        ${GLUT_INCLUDE_DIR}
        ${DEVIL_INCLUDE_DIR}
    )

    target_link_libraries(engine PRIVATE
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARIES}
        ${DEVIL_LIBRARY}
    )

endif()