#!/bin/bash

# Define the program name. Must be the same as CMakeLists.txt
PROGRAM_NAME="OpenGLProject"

# Flags
CLEAN_BUILD=false
RUN_PROGRAM=false

# Parse the command-line arguments for flags
while getopts "dr" opt; do
  case ${opt} in
    d)
      CLEAN_BUILD=true
      ;;
    r)
      RUN_PROGRAM=true
      ;;
    *)
      echo "Usage: $0 [-d (clean build)] [-r (run the program after build)]"
      exit 1
      ;;
  esac
done

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

# Run cmake to configure the project
echo "Running cmake ..."
cmake ..

# Run make to build the project
echo "Running make ..."
make

# Run the program if the -r flag is provided
if [ "$RUN_PROGRAM" = true ]; then
    echo "Running the program..."
    ./$PROGRAM_NAME
else
    echo "Build complete. Use -r flag to run the program."
fi
