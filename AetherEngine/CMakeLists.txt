# CMake version requirement
cmake_minimum_required(VERSION 3.15)

# Add the custom modules path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

# Project name and version
project(AetherEngine VERSION 0.1.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Option to build with GLFW
option(USE_GLFW "Build with GLFW" ON)

# Option to build with WIN32 (default ON for Windows, currently set to off as development is focused on GLFW)
option(USE_WIN32 "Build with WIN32" OFF)

# Option to build with DX11 (default ON for Windows)
option(USE_DX11 "Build with DirectX 11" ON)

# Option to build with Vulkan (default OFF, but ON for Linux)
option(USE_VULKAN "Build with Vulkan" OFF)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include)


# Add key subdirectories
add_subdirectory(RenderingEngine)

# Third-party subdirectories
add_subdirectory(third-party/glfw)

# Link GLFW to your executable
target_link_libraries(AetherRenderer PRIVATE glfw)