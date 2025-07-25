cmake_minimum_required(VERSION 3.10)

project(OpenGLProject)

set(CMAKE_CXX_STANDARD 17)

# Use pkg-config to find GLFW3 and GLEW
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW3 REQUIRED glfw3)
pkg_search_module(GLEW REQUIRED glew)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Add Steamworks paths
set(STEAMWORKS_DIR "${PROJECT_SOURCE_DIR}/third_party/steamworks")
set(STEAM_INCLUDE_DIR "${STEAMWORKS_DIR}/steam")
set(STEAM_LIB_DIR_WIN64 "${STEAMWORKS_DIR}/redistributable_bin/win64")
set(STEAM_LIB_DIR_LINUX64 "${STEAMWORKS_DIR}/redistributable_bin/linux64")
set(STEAM_LIB_DIR_OSX "${STEAMWORKS_DIR}/redistributable_bin/osx")

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
    ${STEAM_INCLUDE_DIR}
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
    target_link_directories(OpenGLProject PRIVATE ${STEAM_LIB_DIR_WIN64})
    target_link_libraries(OpenGLProject
        PRIVATE 
            ${GLFW3_LIBRARIES} 
            ${GLEW_LIBRARIES} 
            OpenGL::GL
            steam_api64
            ws2_32
            winmm
    )

    # Copy steam_api64.dll
    add_custom_command(TARGET OpenGLProject POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${STEAM_LIB_DIR_WIN64}/steam_api64.dll"
        $<TARGET_FILE_DIR:OpenGLProject>
    )
elseif(APPLE)
    target_link_directories(OpenGLProject PRIVATE ${STEAM_LIB_DIR_OSX})
    target_link_libraries(OpenGLProject
        PRIVATE 
            ${GLFW3_LIBRARIES} 
            ${GLEW_LIBRARIES} 
            OpenGL::GL
            steam_api
    )
elseif(UNIX)
    target_link_directories(OpenGLProject PRIVATE ${STEAM_LIB_DIR_LINUX64})
    target_link_libraries(OpenGLProject
        PRIVATE 
            ${GLFW3_LIBRARIES} 
            ${GLEW_LIBRARIES} 
            OpenGL::GL
            steam_api
            pthread
            dl
    )
endif()

# Copy assets (shaders, textures, models)
file(COPY ${PROJECT_SOURCE_DIR}/assets/shaders/ DESTINATION ${CMAKE_BINARY_DIR}/shaders/)
file(COPY ${PROJECT_SOURCE_DIR}/assets/textures/ DESTINATION ${CMAKE_BINARY_DIR}/textures/)
file(COPY ${PROJECT_SOURCE_DIR}/assets/models/ DESTINATION ${CMAKE_BINARY_DIR}/models/)

# Copy steam_appid.txt (make sure it exists in project root)
file(COPY ${PROJECT_SOURCE_DIR}/steam_appid.txt DESTINATION ${CMAKE_BINARY_DIR})