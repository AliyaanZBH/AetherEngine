#
# Set renderer-specific implementations
#


#
# DX11
#

# Specify the source files
set(SOURCES
    main.cpp
    Renderer/D3D/RendererDX11.cpp
    # Add more source files as needed
)

# Define the executable
add_executable(AetherEngine ${SOURCES})

# Include directories for the executable
target_include_directories(AetherEngine PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Platform-specific configurations
if(WIN32 AND USE_DX11)
    # Find DirectX libraries using our "FindX.cmake" files
    find_package(D3D11 REQUIRED)
    find_package(DXGI REQUIRED)
    find_package(D3DCompiler REQUIRED)

    # Link DirectX libraries
    target_link_libraries(AetherEngine PRIVATE D3D11::D3D11 DXGI::DXGI D3DCompiler::D3DCompiler)
    
    # Define a macro for DX11
    target_compile_definitions(AetherEngine PRIVATE USE_DX11)
endif()


#
# Vulkan
#

if(USE_VULKAN)
    find_package(Vulkan REQUIRED)

    if(Vulkan_FOUND)
        message(STATUS "Vulkan SDK found!")
        target_include_directories(AetherEngine PRIVATE ${Vulkan_INCLUDE_DIRS})
        target_link_libraries(AetherEngine PRIVATE Vulkan::Vulkan)
        
        # Define a macro for Vulkan
        target_compile_definitions(AetherEngine PRIVATE USE_VULKAN)
    else()
        message(FATAL_ERROR "Vulkan SDK not found!")
    endif()
endif()

# Handle other platform-specific settings