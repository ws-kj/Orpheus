# CMake version
cmake_minimum_required(VERSION 3.12)

# Project name
project(orpheus)

# Set C++ standard (adjust as needed)
set(CMAKE_CXX_STANDARD 17)

# Set the build type (Release, Debug, etc.)
set(CMAKE_BUILD_TYPE Debug)

# Set the output directory for the build
set(EXECUTABLE_OUTPUT_PATH ${BUILD_DIR})
set(LIBRARY_OUTPUT_PATH ${BUILD_DIR})

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

# Create executable
add_executable(orpheus ${SOURCE_FILES})

