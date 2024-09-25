#
# Set renderer-specific implementations
#

# Specify the source files
set(SRC
    Core/src/CoreApp.cpp
    Core/src/Log.cpp
    GLFW/src/WinManGLFW.cpp
    EngineD3D/src/RendererDX11.cpp
)

set(INCLUDE

    EntryPoint.h

    Core/include/Aether.h
    Core/include/Core.h
    Core/include/CoreApp.h
    Core/include/AetherUtils.h
    Core/include/IRenderer.h
    Core/include/IWindow.h
    Core/include/Log.h

    GLFW/include/WinManGLFW.h

    EngineD3D/include/D3DUtils.h
    EngineD3D/include/RendererDX11.h

    Events/include/Event.h
    Events/include/KeyEvent.h
    Events/include/MouseEvent.h
)


# Define the DLL
add_library(AetherRenderer SHARED ${SRC} ${INCLUDE})

# Include directories for the DLL
target_include_directories(AetherRenderer PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(AetherRenderer PRIVATE include)
target_include_directories(AetherRenderer PRIVATE EngineD3D/include)
target_include_directories(AetherRenderer PRIVATE GLFW/include)
target_include_directories(AetherRenderer PRIVATE Events/include)
# Public so application can see it
target_include_directories(AetherRenderer PUBLIC Core/include)

# Set source groups for VS2022 quality of life
source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})

# Third-party subdirectories
add_subdirectory(ThirdParty/glfw)
add_subdirectory(ThirdParty/spdlog)

# Link third party modules to renderer
target_link_libraries(AetherRenderer PRIVATE glfw)
target_link_libraries(AetherRenderer PUBLIC spdlog::spdlog_header_only)


# Define macro to ensure this project exports a DLL - this is how games can function as their own apps using our rendering code
target_compile_definitions(AetherRenderer PRIVATE AETHER_BUILD_DLL)


#
# Build Configuration specific options
#

# Configuration compile definitions
target_compile_definitions(AetherRenderer PRIVATE $<CONFIG:Debug> AETHER_DEBUG)
target_compile_definitions(AetherRenderer PRIVATE $<CONFIG:Release> AETHER_RELEASE)
target_compile_definitions(AetherRenderer PRIVATE $<CONFIG:Distribution> AETHER_DIST)

# Enable optimisations when not in debug
if (AETHER_RELEASE OR AETHER_DIST)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()


#
# Platform-specific configurations
#

#
# Window type
#

if(WIN32)
    # Set pre-processor macros
    target_compile_definitions(AetherRenderer PUBLIC AETHER_PLATFORM_WINDOWS)
    target_compile_definitions(AetherRenderer PRIVATE USE_WIN32)
endif()

if(USE_GLFW)
    # Set pre-processor macros
    target_compile_definitions(AetherRenderer PRIVATE USE_GLFW)
endif()



#
# DX11
#

if(WIN32 AND USE_DX11)
    # Find DirectX libraries
    find_package(D3D11 REQUIRED)
    find_package(DXGI REQUIRED)
    find_package(D3DCompiler REQUIRED)

    # Link DirectX libraries
    target_link_libraries(AetherRenderer PRIVATE D3D11::D3D11 DXGI::DXGI D3DCompiler::D3DCompiler)
    
    # Define a macro for DX11
    target_compile_definitions(AetherRenderer PRIVATE USE_DX11)

endif()


#
# Vulkan
#

if(USE_VULKAN)
    find_package(Vulkan REQUIRED)

    if(Vulkan_FOUND)
        message(STATUS "Vulkan SDK found!")
        target_include_directories(AetherRenderer PRIVATE ${Vulkan_INCLUDE_DIRS})
        target_link_libraries(AetherRenderer PRIVATE Vulkan::Vulkan)
        
        # Define a macro for Vulkan
        target_compile_definitions(AetherRenderer PRIVATE USE_VULKAN)
    else()
        message(FATAL_ERROR "Vulkan SDK not found!")
    endif()
endif()

# Handle other platform-specific settings
