# Computer Graphics Project

Computer Graphics project developed in C++ for the Computer Graphics course during the third year of my Bachelor's degree in Software Engineering (2025/2026).

The project implements an OpenGL rendering engine and a 3D model generator, with support for hierarchical scenes, lighting, materials, textures, camera control, curves and procedural models.

**Grade:** 19/20

## Authors

- Francisco Contente (https://github.com/contente13)
- Gustavo Braga (https://github.com/gustavocbraga)
- Lucas Pinto (https://github.com/LPP-0)

## Requirements

The project uses CMake, OpenGL, GLUT/freeglut, GLEW and DevIL.

### Ubuntu / Linux

Install a C++ compiler, CMake and the required development libraries:

```bash
sudo apt update
sudo apt install build-essential cmake freeglut3-dev libglew-dev libdevil-dev libglu1-mesa-dev
```

### macOS

Install Apple's Command Line Tools and the required packages with Homebrew:

```bash
xcode-select --install
brew install cmake freeglut glew devil
```

macOS provides the OpenGL framework used by the project.

### Windows

Install [CMake](https://cmake.org/download/) and [Visual Studio](https://visualstudio.microsoft.com/) with the **Desktop development with C++** workload. The GLUT, GLEW and DevIL toolkits are also required.

The Windows CMake configuration expects a common toolkits folder with this structure:

```text
toolkits/
├── glut/   (GL/glut.h, glut32.lib, glut32.dll)
├── glew/   (GL/glew.h, glew32.lib, glew32.dll)
└── devil/  (IL/il.h, DevIL.lib, devil.dll)
```

## Build and Run

Configure and build the project with CMake and Visual Studio (Windows) or the equivalent CMake workflow for Linux/macOS. After building, run the executables from the generated executable folder (`build/Release` on Windows, usually `build` on Linux/macOS).

On Windows PowerShell, run the commands from `build/Release` using the `.\` prefix. For example, to generate a sphere model and run the engine with a scene file:

```powershell
.\generator.exe sphere 1 20 20 sphere.3d
.\engine.exe scenes\solar_fase4.xml
```

On Linux and macOS, run the equivalent commands from the generated executable folder using `./`:

## Controls

- **Arrow keys**: rotate the camera.
- **W/S**: zoom in and out in smaller steps.
- **Z/X**: zoom in and out in larger steps.

## Reports

The phase reports are available in [`reports/`](reports/):

- [Phase 1](reports/Fase%201.pdf)
- [Phase 2](reports/Fase%202.pdf)
- [Phase 3](reports/Fase%203.pdf)
- [Phase 4](reports/Fase%204.pdf)
