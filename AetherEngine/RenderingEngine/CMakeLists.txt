#
# Set renderer-specific implementations
#

# Specify the source files
set(SRC
    
    Core/src/CoreApp.cpp
    
    Core/src/Log.cpp
    GLFW/src/WinManGLFW.cpp
    EngineD3D/src/RendererDX11.cpp
    EngineD3D/src/RendererDX12.cpp
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

    # Renderers
    EngineD3D/include/D3DUtils.h
    EngineD3D/include/RendererDX11.h
    EngineD3D/include/RendererDX12.h


    # Third party includes
    ThirdParty/dx12Helper/include/d3dx12.h
    ThirdParty/dx12Helper/include/ReadData.h
)

# Specify shaders
set(HLSL_SHADER_FILES

    Shaders/VertexShader.hlsl
    Shaders/PixelShader.hlsl
)

set_source_files_properties(${HLSL_SHADER_FILES} PROPERTIES ShaderModel "5_0")
set_source_files_properties(Shaders/VertexShader.hlsl PROPERTIES ShaderType "vs")
set_source_files_properties(Shaders/PixelShader.hlsl PROPERTIES ShaderType "ps")


# Define the DLL
add_library(AetherRenderer SHARED ${SRC} ${INCLUDE} ${HLSL_SHADER_FILES} "ThirdParty/dx12Helper/include/ReadData.h")

# Include directories for the DLL
target_include_directories(AetherRenderer PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(AetherRenderer PRIVATE include)
target_include_directories(AetherRenderer PRIVATE EngineD3D/include)
target_include_directories(AetherRenderer PRIVATE GLFW/include)
target_include_directories(AetherRenderer PRIVATE ThirdParty/dx12Helper/include)

# Public so application can see it
target_include_directories(AetherRenderer PUBLIC Core/include)


# Set source groups for VS2022 quality of life
source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("shaders" FILES ${HLSL_SHADER_FILES})

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
# Direct X
#

if(WIN32 AND (USE_DX11 OR USE_DX12))
    # Find DirectX common libraries
    find_package(DXGI REQUIRED)
    find_package(D3DCompiler REQUIRED)
    find_package(DXGUID REQUIRED)

    # Find specific feature level package, link and set macro in our renderer
    if (USE_DX11)
        find_package(D3D11 REQUIRED)
        target_link_libraries(AetherRenderer PRIVATE D3D11::D3D11 DXGI::DXGI D3DCompiler::D3DCompiler)
        target_compile_definitions(AetherRenderer PRIVATE USE_DX11)
    endif()
    
    if(USE_DX12)
        find_package(D3D12 REQUIRED)
        target_link_libraries(AetherRenderer PRIVATE D3D12::D3D12 DXGI::DXGI D3DCompiler::D3DCompiler DXGUID::DXGUID)   # Note the need for DXGUID here
        target_compile_definitions(AetherRenderer PRIVATE USE_DX12)
    endif()
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



# Build HLSL shaders
foreach(FILE ${HLSL_SHADER_FILES})
    get_filename_component(FILE_WE ${FILE} NAME_WE)
    list(APPEND CSO_SHADER_FILES ${CMAKE_BINARY_DIR}/${FILE_WE}.cso)
    get_source_file_property(shadertype ${FILE} ShaderType)
    get_source_file_property(shadermodel ${FILE} ShaderModel)

    # Copy the raw shader files across to the DX folder
    ##add_custom_command(TARGET AetherRenderer PRE_LINK
    ##                   COMMAND ${CMAKE_COMMAND} -E copy ${CSO_SHADER_FILES} $<TARGET_FILE_DIR:AetherRenderer/EngineD3D/src>
    ##                   COMMAND_EXPAND_LISTS
    ##)

    add_custom_command(TARGET AetherRenderer PRE_LINK
                       COMMAND dxc.exe /nologo /Emain /T ${shadertype}_${shadermodel} /Od /Zi /Fo ${CMAKE_BINARY_DIR}/${FILE_WE}.cso /Fd ${CMAKE_BINARY_DIR}/${FILE_WE}.pdb ${FILE}
                       COMMENT "HLSL ${FILE}"
                       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                       VERBATIM)
endforeach(FILE)

# Make CSO list public and visible to our EXE so that it can copy it post-build
set_property(GLOBAL PROPERTY CSO_FILES "${CSO_SHADER_FILES}")

# Copy CSOs to DLL folder in build step
add_custom_command(TARGET AetherRenderer POST_BUILD
                  COMMAND ${CMAKE_COMMAND} -E copy ${CSO_SHADER_FILES} $<TARGET_FILE_DIR:AetherRenderer>
                  COMMAND_EXPAND_LISTS
)
