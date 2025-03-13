#!/bin/bash

# Define the program name. Must be the same as CMakeList.txt
PROGRAM_NAME="OpenGLProject"

# Flag for cleaning the build directory
CLEAN_BUILD=false

# Parse the command-line arguments for flags
while getopts "r" opt; do
  case ${opt} in
    r)
      CLEAN_BUILD=true
      ;;
    *)
      echo "Usage: $0 [-r]"
      exit 1
      ;;
  esac
done

# If the -r flag is provided, clean the build directory
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

# Run the program
echo "Running the program..."
./$PROGRAM_NAME
