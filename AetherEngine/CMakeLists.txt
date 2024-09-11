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


# This is a flag that will flip whether or not any of our projects are importing or exporting a dll
#option(AETHER_BUILD_DLL "Used to distinguish when the rendering API needs to export its DLL or if an application is importing it." ON)



#option(AETHER_PLATFORM_WINDOWS "Turn this on if you're targeting Windows'." ON)

# Option to build with GLFW
option(USE_GLFW "Create windows with GLFW" ON)
# Option to build with WIN32 API (default ON for Windows, currently set to off as development is focused on GLFW)
option(USE_WIN32 "Create windows with WIN32" OFF)

# Option to build with DX11 (default ON for Windows)
option(USE_DX11 "Build with DirectX 11" ON)

# Option to build with Vulkan (default OFF, but ON for Linux)
option(USE_VULKAN "Build with Vulkan" OFF)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include)


# Add key subdirectories
add_subdirectory(RenderingEngine)
add_subdirectory(EngineApplication)

# Link renderer DLL to Application exe
target_link_libraries(AetherApp PRIVATE AetherRenderer)