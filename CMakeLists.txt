cmake_minimum_required(VERSION 3.1)

project(Observer)

# Specify output directories.
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")

# Build with c++14 support, required by sc2api.
set(CMAKE_CXX_STANDARD 14)

# Disable building of examples in the cpp-sc2 submodule.
set(BUILD_API_EXAMPLES OFF CACHE INTERNAL "" FORCE)

# Disable building of tests in the cpp-sc2 submodule.
set(BUILD_API_TESTS OFF CACHE INTERNAL "" FORCE)

add_subdirectory("cpp-sc2")
add_subdirectory("src")
