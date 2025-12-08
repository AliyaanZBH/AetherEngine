# CMake version requirement
cmake_minimum_required(VERSION 3.16)

# Add the custom modules path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

# Project name and version
project(AetherEngine VERSION 0.1.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Setup build configs
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo " CACHE STRING "" FORCE)


# Option to build with GLFW
option(USE_GLFW "Create windows with GLFW" ON)
# Option to build with WIN32 API (default ON for Windows, currently set to off as development is focused on GLFW)
option(USE_WIN32 "Create windows with WIN32" OFF)

# Option to build with DX11
option(USE_DX11 "Build with DirectX 11" OFF)

# Option to build with DX12 (default ON for Windows)
option(USE_DX12 "Build with DirectX 12" ON)

# Option to build with Vulkan (default OFF, but ON for Linux)
option(USE_VULKAN "Build with Vulkan" OFF)


# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Set a unified folder for runtime binaries (DLLs, EXEs, PDBs)
set(BIN_DIR ${CMAKE_BINARY_DIR}/bin)

# All targets (DLL/EXE) will output here
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR})  # For static libs from third-parties

# Ensure multi-config support (Debug/Release)
foreach(OUTPUT_CONFIG Debug Release RelWithDebInfo Distribution)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} ${CMAKE_BINARY_DIR}/bin)
endforeach()

# Add main projects as indivudual subdirectories as they have their own CMakeLists file. These will build in order, so engine DLL first then the app (which depends on the DLL!)
add_subdirectory(RenderingEngine)
add_subdirectory(EngineApplication)
