#
# Set renderer-specific implementations
#

# Specify the precompiled header file first
set(PCH_HEADER "PCH/AetherPCH.h")

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
    Core/include/Log.h

    # Interfaces
    Core/include/IRenderer.h
    Core/include/IWindow.h

    # Window
    GLFW/include/WinManGLFW.h

    # Rendering
    EngineD3D/include/D3DUtils.h
    EngineD3D/include/RendererDX11.h
    EngineD3D/include/RendererDX12.h

    Events/include/Event.h
    Events/include/KeyEvent.h
    Events/include/MouseEvent.h
    Events/include/AppEvent.h


    # Third party includes
    ThirdParty/dx12Helper/include/d3dx12.h
)

# Specify shaders
set(HLSL_SHADER_FILES

    Shaders/VertexShader.hlsl
    Shaders/PixelShader.hlsl
)

set_source_files_properties(${HLSL_SHADER_FILES} PROPERTIES ShaderModel "5_0")
set_source_files_properties(Shaders/VertexShader.hlsl PROPERTIES ShaderType "vs")
set_source_files_properties(Shaders/PixelShader.hlsl PROPERTIES ShaderType "ps")


# Define the DLL target
add_library(AetherRenderer SHARED ${SRC} ${INCLUDE} ${HLSL_SHADER_FILES})

# Enable PCH for Visual Studio
if (MSVC)  # Check if we're on Visual Studio
    target_precompile_headers(AetherRenderer PRIVATE ${PCH_HEADER})
endif()

# Include directories for the DLL
target_include_directories(AetherRenderer
                    PRIVATE 
                            ${PROJECT_SOURCE_DIR}/include
                            EngineD3D/include
                            GLFW/include
                            Events/include
                            ThirdParty/dx12Helper/include
                    PUBLIC
                         Core/include # Public as application target needs to see it too
)


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
target_compile_definitions(AetherRenderer PRIVATE 
    $<CONFIG:Debug> AETHER_DEBUG
    $<CONFIG:Release> AETHER_RELEASE
    $<CONFIG:RelWithDebInfo> AETHER_RELEASE_DBG_INFO
    $<CONFIG:Distribution> AETHER_DIST
)

# Enable optimisations when not in debug
#if (AETHER_RELEASE OR AETHER_DIST)
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug" )
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
        target_compile_definitions(AetherRenderer PUBLIC USE_DX11)
    endif()
    
    if(USE_DX12)
        find_package(D3D12 REQUIRED)
        target_link_libraries(AetherRenderer PRIVATE D3D12::D3D12 DXGI::DXGI D3DCompiler::D3DCompiler DXGUID::DXGUID)   # Note the need for DXGUID here
        target_compile_definitions(AetherRenderer PUBLIC USE_DX12)
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

    # Create list of compiled and raw shader files
    list(APPEND RAW_SHADER_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/${FILE})
    list(APPEND CSO_SHADER_FILES ${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.cso)
    
    get_source_file_property(shadertype ${FILE} ShaderType)
    get_source_file_property(shadermodel ${FILE} ShaderModel)
                      
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.cso
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND dxc.exe /nologo /E main /T ${shadertype}_${shadermodel}
                /Od /Zi /Fo ${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.cso 
                /Fd ${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.pdb ${FILE}
        DEPENDS ${FILE}
        COMMENT "Compiling HLSL ${FILE}..."
        COMMAND ${CMAKE_COMMAND} -E echo "Successfuly compiled shader!"
        VERBATIM
    )
endforeach(FILE)


# Create a target for shaders - using snake_case to denote that this is an intermediate target only
add_custom_target(aether_shaders DEPENDS ${CSO_SHADER_FILES})

# Make renderer depend on shader compilation
add_dependencies(AetherRenderer aether_shaders)

# Deployment target (DLL + PDB + shaders)
set(RUNTIME_DIR "$<TARGET_FILE_DIR:AetherRenderer>")
set(SHADER_DEST "${RUNTIME_DIR}/Shaders")


# Copy raw shaders and CSOs to bin folder in build step via another custom intermediate target
add_custom_target(deploy_shaders ALL
    DEPENDS AetherRenderer aether_shaders
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_DEST}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CSO_SHADER_FILES} ${SHADER_DEST} COMMAND_EXPAND_LISTS
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RAW_SHADER_FILES} ${SHADER_DEST} COMMAND_EXPAND_LISTS
    COMMENT "Deploying AetherEngine shaders..."
    COMMAND ${CMAKE_COMMAND} -E echo "Engine shader files deployed!"
)
