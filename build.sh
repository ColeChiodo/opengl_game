#!/bin/bash

# Define the program name. Must be the same as CMakeLists.txt
PROGRAM_NAME="OpenGLProject"

# Flags
CLEAN_BUILD=false
RUN_PROGRAM=false
PLATFORM=""

# Parse the command-line arguments for flags
while getopts "dlrw" opt; do
  case ${opt} in
    d)
      CLEAN_BUILD=true
      ;;
    r)
      RUN_PROGRAM=true
      ;;
    l)
      PLATFORM="linux"
      ;;
    w)
      PLATFORM="windows"
      ;;
    *)
      echo "Usage: $0 [-d (clean build)] [-r (run the program after build)] [-l (build for Linux)] [-w (build for Windows)]"
      exit 1
      ;;
  esac
done

# Ensure a platform is selected
if [ -z "$PLATFORM" ]; then
  echo "You must specify a platform: -l for Linux or -w for Windows."
  exit 1
fi

# If the -d flag is provided, clean the build directory
if [ "$CLEAN_BUILD" = true ]; then
    if [ -d "build" ]; then
        echo "Cleaning the build directory..."
        rm -rf ./build/*
    else
        echo "Build directory does not exist, no cleanup needed."
    fi
else
    # If no flag is passed, just create the build directory if it doesn't exist
    if [ ! -d "build" ]; then
        echo "Build directory does not exist. Creating 'build' directory..."
        mkdir build
    fi
fi

# Navigate to the build directory
cd build || exit

# Configure the build depending on the platform
echo "Running cmake for platform: $PLATFORM..."

if [ "$PLATFORM" = "linux" ]; then
    cmake ..
    BUILD_CMD="make"
elif [ "$PLATFORM" = "windows" ]; then
    cmake -G "Visual Studio 16 2019" ..
    BUILD_CMD=""

    echo "Visual Studio solution generated. Open the .sln file with Visual Studio to build the project."
fi

# Run the build command (using Visual Studio on Windows or make on Linux)
if [ "$PLATFORM" = "linux" ]; then
    echo "Running make ..."
    $BUILD_CMD
fi

# Run the program if the -r flag is provided
if [ "$RUN_PROGRAM" = true ] && [ "$PLATFORM" = "linux" ]; then
    echo "Running the program..."
    ./$PROGRAM_NAME
else
    echo "Build complete. Use -r flag to run the program."
fi
