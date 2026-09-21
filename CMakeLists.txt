cmake_minimum_required(VERSION 3.16)
project(TaxiVis)
include(CTest)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")

# Add subdirectories
add_subdirectory(src/preprocess)
add_subdirectory(src/TaxiVis)
