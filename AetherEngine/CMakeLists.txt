# CMake version requirement
cmake_minimum_required(VERSION 3.16)

# Add the custom modules path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

# Project name and version
project(AetherEngine VERSION 0.1.0 LANGUAGES C CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Setup build configs - config types for multi config and build types for single config
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;Shipping " CACHE STRING "" FORCE)
set(CMAKE_BUILD_TYPES "Debug;Release;RelWithDebInfo;Shipping " CACHE STRING "" FORCE)

# Global compiler options
set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} /MD /O2")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MD /O2")
set(CMAKE_CXX_FLAGS_SHIPPING       "${CMAKE_CXX_FLAGS_RELEASE} /O2 /GL")


# Option to build with GLFW
option(USE_GLFW "Create windows with GLFW" ON)   # GLFW runs everywhere so we can default this to on!
# Option to build with WIN32 API (default ON for Windows, currently set to off as development is focused on GLFW)
option(USE_WIN32 "Create windows with WIN32" OFF)


# Option to build with OpenGL
option(BUILD_OPENGL "Build with OpenGL (and GLAD)" ON)        # OpenGL runs everywhere so we can default this to on!
option(USE_OPENGL "Compile with OpenGL renderer" ON)

# Option to build with DX11
option(BUILD_DX11 "Build with DirectX 11" ON)
option(USE_DX11 "Compile with DX11 renderer" OFF)

# Option to build with DX12 (default ON for Windows)
option(BUILD_DX12 "Build with DirectX 12" ON)
option(USE_DX12 "Compile with DX12 renderer" OFF)

# Option to build with Vulkan (default OFF, but ON for Linux)
option(BUILD_VULKAN "Build with Vulkan" OFF)
option(USE_VULKAN "Compile with Vulkan renderer" OFF)


# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Set a unified folder for runtime binaries (DLLs, EXEs, PDBs)
set(BIN_DIR ${CMAKE_BINARY_DIR}/bin)

# All targets (DLL/EXE) will output here
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR}/lib)  # static libs are treated as archive files

# Ensure multi-config support (Debug/Release)
foreach(OUTPUT_CONFIG Debug Release RelWithDebInfo Shipping)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} ${BIN_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} ${BIN_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} ${BIN_DIR}/lib)
endforeach()


# Intermediate interface target to handle config-specific macros across engine, app, and third-party libs, ensuring we don't get ABI or symbol mismatch
add_library(AetherConfig INTERFACE)
# Neat little debug helper to check our includer order when things are being weird
# target_compile_options(AetherConfig INTERFACE /showIncludes)

target_compile_definitions(AetherConfig INTERFACE
    $<$<CONFIG:Debug>:AETHER_DEBUG>
    $<$<CONFIG:Release>:AETHER_RELEASE>
    $<$<CONFIG:RelWithDebInfo>:AETHER_RELEASE_DBG_INFO>
    $<$<CONFIG:Shipping>:AETHER_SHIP>

    # Dynamic linking for everything EXCEPT Shipping
    $<$<NOT:$<CONFIG:Shipping>>:AETHER_DYNAMIC_LINK>
)

# Add main projects as indivudual subdirectories as they have their own CMakeLists file. These will build in order, so engine DLL first then the app (which depends on the DLL!)
add_subdirectory(RenderingEngine)
add_subdirectory(EngineApplication)
