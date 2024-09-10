#
# Set renderer-specific implementations
#


# Specify the source files
set(SRC
    main.cpp
    GLFW/src/WinManGLFW.cpp
    EngineD3D/src/RendererDX11.cpp
)

set(INCLUDE
    Common/include/AetherUtils.h
    Common/include/IRenderer.h
    Common/include/IWindow.h
    GLFW/include/WinManGLFW.h
    EngineD3D/include/D3DUtils.h
    EngineD3D/include/RendererDX11.h
)

# Include directories for source code
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(EngineD3D/include)
include_directories(Common/include)
include_directories(GLFW/include)

# Define the executable
add_executable(AetherRenderer ${SRC} ${INCLUDE})

# Include directories for the executable
target_include_directories(AetherRenderer PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(AetherRenderer PRIVATE include)
target_include_directories(AetherRenderer PUBLIC Common/include)
target_include_directories(AetherRenderer PUBLIC GLFW/include)
target_include_directories(AetherRenderer PUBLIC EngineD3D/include)


# Set source groups for VS2022 quality of life
source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})

#
# Platform-specific configurations
#

#
# Window type
#

if(WIN32)
    # Set pre-processor macros
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