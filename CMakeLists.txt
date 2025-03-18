cmake_minimum_required(VERSION 3.10)

project(OpenGLProject)

set(CMAKE_CXX_STANDARD 17)

# Use pkg-config to find GLFW3 and GLEW
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW3 REQUIRED glfw3)
pkg_search_module(GLEW REQUIRED glew)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Add include directories (for both headers and libraries)
include_directories(${GLFW3_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/lib/include)

# Add your source files (main.cpp, shader.cpp, stb_image.cpp, etc.)
file(GLOB SOURCES 
    src/*.cpp
)

# Add stb_image.cpp explicitly (or you can just use the globbing pattern to include it)
list(APPEND SOURCES src/stb_image.cpp)

# Create the executable
add_executable(OpenGLProject ${SOURCES})

# Add Definition for Enabling GLM Experiemental Mode
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

# Link the libraries
target_link_libraries(OpenGLProject ${GLFW3_LIBRARIES} ${GLEW_LIBRARIES} OpenGL::GL)

# Copy the shaders directory to the output directory (e.g., bin or build)
# This ensures that shaders are available during runtime
file(COPY ${PROJECT_SOURCE_DIR}/assets/shaders/ DESTINATION ${CMAKE_BINARY_DIR})

# Copy the textures directory to the output directory (e.g., bin or build)
# This ensures that textures are available during runtime
file(COPY ${PROJECT_SOURCE_DIR}/assets/textures/ DESTINATION ${CMAKE_BINARY_DIR})

# Copy the models directory to the output directory (e.g., bin or build)
# This ensures that textures are available during runtime
file(COPY ${PROJECT_SOURCE_DIR}/assets/models/ DESTINATION ${CMAKE_BINARY_DIR}/models/)
