cmake_minimum_required(VERSION 3.10)

project(Observer)

# Specify output directories.
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")

# Build with c++14 support required by sc2api.
# Build with c++17 support required by ToolsCommon.
set(CMAKE_CXX_STANDARD 17)

# Disable building of examples in the cpp-sc2 submodule.
set(BUILD_API_EXAMPLES OFF CACHE INTERNAL "" FORCE)

# Disable building of tests in the cpp-sc2 submodule.
set(BUILD_API_TESTS OFF CACHE INTERNAL "" FORCE)

# Search for Google test suite.
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

if (MSVC)
    # Setup MSVC parallelized builds.
    add_compile_options(/MP)

    # Use statically linked runtime.
    set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>)
endif ()

add_subdirectory("cpp-sc2")
add_subdirectory("src")
add_subdirectory("test")
