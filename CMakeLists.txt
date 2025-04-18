cmake_minimum_required(VERSION 3.10)

project(OpenGLProject)

set(CMAKE_CXX_STANDARD 17)

# Use pkg-config to find GLFW3 and GLEW
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW3 REQUIRED glfw3)
pkg_search_module(GLEW REQUIRED glew)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Add include directories
include_directories(
    ${GLFW3_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/core
    ${PROJECT_SOURCE_DIR}/src/renderer
    ${PROJECT_SOURCE_DIR}/src/game
    ${PROJECT_SOURCE_DIR}/src/audio
    ${PROJECT_SOURCE_DIR}/src/input
    ${PROJECT_SOURCE_DIR}/src/network
    ${PROJECT_SOURCE_DIR}/src/physics
    ${PROJECT_SOURCE_DIR}/src/ui
    ${PROJECT_SOURCE_DIR}/third_party
    ${PROJECT_SOURCE_DIR}/third_party/enet
)

# Add source files
file(GLOB SOURCES 
    src/*.cpp
    src/core/*.cpp
    src/renderer/*.cpp
    src/game/*.cpp
    src/audio/*.cpp
    src/input/*.cpp
    src/network/*.cpp
    src/physics/*.cpp
    src/ui/*.cpp
    third_party/imgui/*.cpp
)

# Add stb_image.cpp explicitly
list(APPEND SOURCES src/renderer/stb_image.cpp)

# Create the executable
add_executable(OpenGLProject ${SOURCES})

# Add Definition for Enabling GLM Experimental Mode
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

# Link libraries based on platform
if(WIN32)
    target_link_libraries(OpenGLProject
        PRIVATE 
            ${GLFW3_LIBRARIES} 
            ${GLEW_LIBRARIES} 
            OpenGL::GL
            ws2_32
            winmm
    )
elseif(UNIX)
    target_link_libraries(OpenGLProject
        PRIVATE 
            ${GLFW3_LIBRARIES} 
            ${GLEW_LIBRARIES} 
            OpenGL::GL
            pthread
            dl
    )
endif()

# Copy assets (shaders, textures, models)
file(COPY ${PROJECT_SOURCE_DIR}/assets/shaders/ DESTINATION ${CMAKE_BINARY_DIR}/shaders/)
file(COPY ${PROJECT_SOURCE_DIR}/assets/textures/ DESTINATION ${CMAKE_BINARY_DIR}/textures/)
file(COPY ${PROJECT_SOURCE_DIR}/assets/models/ DESTINATION ${CMAKE_BINARY_DIR}/models/)
