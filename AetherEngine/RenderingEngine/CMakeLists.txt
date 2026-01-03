#
# Set renderer-specific implementations
#

# Specify the precompiled header file first
set(PCH_HEADER "PCH/AetherPCH.h")

# Specify the source files
set(SRC
    
    Core/src/CoreApp.cpp 

    Core/src/Log.cpp
    Core/src/Camera.cpp
    Core/src/Window.cpp
    Core/src/Renderer.cpp
    Core/src/LayerStack.cpp

    Input/src/Input.cpp

    Window/src/WindowGLFW.cpp

    #
    # Graphics Primitives
    #

    Graphics/src/Shader.cpp

    #
    # DirectX Implementations
    #

    RendererBackend/DirectX/src/RendererDX11.cpp
    RendererBackend/DirectX/src/BufferDX11.cpp
    RendererBackend/DirectX/src/ShaderDX11.cpp

    RendererBackend/DirectX/src/RendererDX12.cpp
    RendererBackend/DirectX/src/BufferDX12.cpp
    RendererBackend/DirectX/src/ShaderDX12.cpp

    #
    # OpenGL Implementations
    #

    RendererBackend/OpenGL/src/RendererOpenGL.cpp
    RendererBackend/OpenGL/src/ShaderOpenGL.cpp
    RendererBackend/OpenGL/src/BufferOpenGL.cpp

    Layers/src/ImGuiLayer.cpp
)

set(INCLUDE

    EntryPoint.h

    # Core
    Core/include/Aether.h
    Core/include/Core.h
    Core/include/AetherUtils.h
    Core/include/CoreApp.h
    Core/include/Camera.h
    Core/include/Log.h
    Core/include/Event.h
    Core/include/Layer.h
    Core/include/LayerStack.h
    Core/include/Renderer.h
    Core/include/Window.h
    
    # Input
    Input/include/InputCodes.h
    Input/include/Input.h



    # Interfaces
    RendererBackend/interface/IRendererBackend.h
    Window/interface/IWindow.h

    Window/include/WindowContext.h
    Window/include/WindowGLFW.h

    #
    # Graphics Primitives
    #

    Graphics/include/GraphicsContext.h
    Graphics/include/DrawCommand.h
    Graphics/include/GraphicsCommon.h
    Graphics/include/Shader.h
    Graphics/include/Buffer.h
    Graphics/include/Vertex.h
    Graphics/include/Pipeline.h

    #
    # DirectX
    #

    RendererBackend/DirectX/include/D3DUtils.h
    
    RendererBackend/DirectX/include/RendererDX11.h
    RendererBackend/DirectX/include/BufferDX12.h
    RendererBackend/DirectX/include/ShaderDX11.h
    
    RendererBackend/DirectX/include/RendererDX12.h
    RendererBackend/DirectX/include/ImGuiDX12.h
    RendererBackend/DirectX/include/LinearAllocatorDX12.h
    RendererBackend/DirectX/include/BufferDX12.h
    RendererBackend/DirectX/include/ShaderDX12.h

    #
    # OpenGL
    #

    RendererBackend/OpenGL/include/RendererOpenGL.h
    RendererBackend/OpenGL/include/ShaderOpenGL.h
    RendererBackend/OpenGL/include/BufferOpenGL.h

    # Events
    Events/include/EventContext.h
    Events/include/KeyEvent.h
    Events/include/MouseEvent.h
    Events/include/AppEvent.h

    # Layers
    Layers/include/ImGuiLayer.h

    # Drawing
    Drawing/include/Drawing.h


    # Miscellaneous third-party includes
    ThirdParty/dx12Helper/include/d3dx12.h
 )

# Specify shaders
set(HLSL_SHADER_FILES

    Shaders/DirectX/VertexShader.hlsl
    Shaders/DirectX/PixelShader.hlsl
)

# Extra steps for HLSL and MSVC
set_source_files_properties(${HLSL_SHADER_FILES} PROPERTIES ShaderModel "5_0")
set_source_files_properties(Shaders/DirectX/VertexShader.hlsl PROPERTIES ShaderType "vs")
set_source_files_properties(Shaders/DirectX/PixelShader.hlsl PROPERTIES ShaderType "ps")

# Repeat for GLSL
set(GLSL_SHADER_FILES

    Shaders/OpenGL/VertexShader.glsl
    Shaders/OpenGL/PixelShader.glsl
)



# Define the target - we want a DLL for development and a static lib (for the juicy optimisations) when shipping.
# Decide library type based on config
message( ${CMAKE_BUILD_TYPE})
if(CMAKE_BUILD_TYPE STREQUAL "Shipping")
    add_library(AetherRenderer STATIC ${SRC} ${INCLUDE})
else()
    add_library(AetherRenderer SHARED ${SRC} ${INCLUDE})
    
    # This macro ensures that engine functions are exported when building a DLL - This is undefined in the application and which will import them instead! And when static linking, this define gets superseded and is ignored. 
    # See Core.h for implementation
    target_compile_definitions(AetherRenderer PRIVATE AETHER_BUILD_DLL)
endif()

# Link config to propogate configuration macros
target_link_libraries(AetherRenderer PRIVATE AetherConfig)

# Enable PCH for Visual Studio
if (MSVC) 
    target_precompile_headers(AetherRenderer PRIVATE ${PCH_HEADER})
    # Neat little debug helper to check our includer order when things are being weird
    #target_compile_options(AetherRenderer PRIVATE /showIncludes)
    #target_compile_options(AetherRenderer PRIVATE /sourceDependencies)
endif()

# Include directories for the DLL
target_include_directories(AetherRenderer
                    # Certain things need to be public as the application target needs to see these interfaces. Detailed implementations can remain private
                    PUBLIC
                            Core/include 
                            Graphics/include
                            Drawing/include
                            Input/include

                    PRIVATE 
                            ${PROJECT_SOURCE_DIR}/include
                            Window/interface
                            Window/include
                            RendererBackend/interface
                            RendererBackend/DirectX/include
                            RendererBackend/OpenGL/include
                            Events/include
                            Layers/include
                            ThirdParty/dx12Helper/include
)


# Set source groups for VS2022 quality of life
source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("shaders" FILES ${HLSL_SHADER_FILES} ${GLSL_SHADER_FILES})

#
# Third-party libraries and subdirectories (subdirectories only for projects that contain a CMakeLists file)
#

if(USE_GLFW)
    add_subdirectory(ThirdParty/glfw EXCLUDE_FROM_ALL)  # EXCLUDE ensures we only build this subdirectory once!
    target_link_libraries(AetherRenderer PRIVATE glfw)
    # Set pre-processor macro in target
    target_compile_definitions(AetherRenderer PRIVATE USE_GLFW)
endif()

add_subdirectory(ThirdParty/spdlog EXCLUDE_FROM_ALL)
add_subdirectory(ThirdParty/glad)
add_subdirectory(ThirdParty/glm)
add_subdirectory(ThirdParty/imgui-1.92.5)

# Link third party modules to renderer
target_link_libraries(AetherRenderer PRIVATE glfw)
target_link_libraries(AetherRenderer PRIVATE glad)

# These ones are public since the application will want to be able to do some maths or logging or create ImGui windows for use in the editor.
target_link_libraries(AetherRenderer PUBLIC glm::glm-header-only)
target_link_libraries(AetherRenderer PUBLIC spdlog::spdlog_header_only)
target_link_libraries(AetherRenderer PUBLIC imgui)

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


#
# Direct X
#

if(WIN32 AND (BUILD_DX11 OR BUILD_DX12))
    # Find DirectX common libraries
    find_package(DXGI REQUIRED)
    find_package(D3DCompiler REQUIRED)
    find_package(DXGUID REQUIRED)

    # Find specific feature level package, link and set macro in our renderer
    if (BUILD_DX11)
        find_package(D3D11 REQUIRED)
        target_link_libraries(AetherRenderer PRIVATE D3D11::D3D11 DXGI::DXGI D3DCompiler::D3DCompiler)
        target_compile_definitions(AetherRenderer PUBLIC USE_DX11)
    endif()
    
    if(BUILD_DX12)
        find_package(D3D12 REQUIRED)
        target_link_libraries(AetherRenderer PRIVATE D3D12::D3D12 DXGI::DXGI D3DCompiler::D3DCompiler DXGUID::DXGUID)   # Note the need for DXGUID here
        target_compile_definitions(AetherRenderer PUBLIC USE_DX12)
    endif()
endif()


#
# Open GL & Vulkan
#

if(BUILD_OPENGL)
    target_compile_definitions(AetherRenderer PRIVATE USE_OPENGL)
endif()

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
    list(APPEND RAW_HLSL_SHADER_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/${FILE})
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
        COMMAND ${CMAKE_COMMAND} -E echo "Successfuly compiled HLSL shader!"
        VERBATIM
    )
endforeach(FILE)

# Create a list of full paths to our GLSL files too
foreach(FILE ${GLSL_SHADER_FILES})
    list(APPEND RAW_GLSL_SHADER_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/${FILE})
endforeach(FILE)

# Create a target for shaders - using snake_case to denote that this is an intermediate target only
add_custom_target(aether_shaders DEPENDS ${CSO_SHADER_FILES} ${GLSL_SHADER_FILES})

# Make renderer depend on shader compilation
add_dependencies(AetherRenderer aether_shaders)

# Deployment target for the shaders - use the top level binary directory that we defined.
set(SHADER_DEST_DIRECTX "${BIN_DIR}/Shaders/DirectX")
set(SHADER_DEST_OPENGL "${BIN_DIR}/Shaders/OpenGL")


# Copy raw shaders and CSOs to bin folder in build step via another custom intermediate target
add_custom_target(deploy_shaders ALL
    DEPENDS AetherRenderer aether_shaders
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_DEST_DIRECTX}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_DEST_OPENGL}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CSO_SHADER_FILES} ${SHADER_DEST_DIRECTX} COMMAND_EXPAND_LISTS
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RAW_HLSL_SHADER_FILES} ${SHADER_DEST_DIRECTX} COMMAND_EXPAND_LISTS
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RAW_GLSL_SHADER_FILES} ${SHADER_DEST_OPENGL} COMMAND_EXPAND_LISTS
    COMMENT "Deploying AetherEngine shaders..."
    COMMAND ${CMAKE_COMMAND} -E echo "Engine shader files deployed!"
)

