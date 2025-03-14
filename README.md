# OpenGL Project

This project is a basic OpenGL game.

## Requirements

### Required Libraries

Before running the project, you need to install the following libraries:

1. **GLFW3**
   - GLFW is used for creating windows with OpenGL contexts and handling input events like keyboard and mouse.
   - Install via package manager (Linux):
     ```bash
     sudo apt-get install libglfw3 libglfw3-dev
     ```

2. **GLEW**
   - GLEW is used for managing OpenGL extensions.
   - Install via package manager (Linux):
     ```bash
     sudo apt-get install libglew-dev
     ```

3. **OpenGL**
   - OpenGL is the graphics rendering API used in this project.
   - Install via package manager (Linux):
     ```bash
     sudo apt-get install libopengl-dev
     ```

4. **Pkg-config**
   - Pkg-config helps manage library compile and link flags.
   - Install via package manager (Linux):
     ```bash
     sudo apt-get install pkg-config
     ```

### Additional Tools

- **CMake** (for building the project)
   - Install via package manager (Linux):
     ```bash
     sudo apt-get install cmake
     ```

- **Optional**: On Windows, you may need to use MSYS2 or WSL to provide the Unix-like environment to use `pkg-config` and other tools. 

## Setup and Build

### Step 1: Clone the repository

Clone this repository to your local machine:

```bash
git clone https://github.com/ColeChiodo/opengl_game.git
cd opengl_game
```

### Step 2: Run the build script

The project comes with a build.sh script that will handle the configuration and compilation of the project using CMake.

Make the script executable:

```bash
chmod +x build.sh
```

Now, run the script with the appropriate flags:

```bash
./build.sh [-d] [-r]
```

Available flags:

- `-d` : Delete the contents of the build directory before building the project.
- `-r` : Run the program after building it.

Alernatively, run the program using:
```bash
cd ./build
./OpenGLProject
```
