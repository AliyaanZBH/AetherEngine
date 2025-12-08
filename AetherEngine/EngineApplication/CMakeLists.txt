#
# Main application for Aether
#

# Include directories for source code
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/RenderingEngine/)


set(SRC
    main.cpp
)

set(INCLUDE
    ${PROJECT_SOURCE_DIR}/RenderingEngine/Core/include/Aether.h
)


# Define the executable
add_executable(AetherApp ${SRC} ${INCLUDE})

# Include directories for the executable
target_include_directories(AetherApp PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(AetherApp PRIVATE include)
target_include_directories(AetherApp PRIVATE include)

# Set source groups for VS2022 quality of life
source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})


#
# Build Configuration specific options
#


# Configuration compile definitions
target_compile_definitions(AetherApp PRIVATE $<CONFIG:Debug> AETHER_DEBUG)
target_compile_definitions(AetherApp PRIVATE $<CONFIG:Release> AETHER_RELEASE)
target_compile_definitions(AetherApp PRIVATE $<CONFIG:RelWithDebInfo> AETHER_RELEASE_DBG_INFO)
target_compile_definitions(AetherApp PRIVATE $<CONFIG:Distribution> AETHER_DIST)

# Enable optimisations when not in debug
if (AETHER_RELEASE OR AETHER_DIST)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()


# Also explicitly add the DLL as dependency - this ensures that when running the exe, the DLL gets recompiled too :)
add_dependencies(AetherApp AetherRenderer)

# Link renderer DLL to our app exe
target_link_libraries(AetherApp PRIVATE AetherRenderer)

# Pre-build step to ensure that the dll is up to date!
add_custom_command(TARGET AetherApp PRE_BUILD
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target AetherRenderer
    COMMENT "Building engine Dll before running app .exe!"
)
message(STATUS "DLL Built")

# Post-build step to copy the DLL to the directory where the .exe is - huge time saver!
add_custom_command(TARGET AetherApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:AetherRenderer>               # The path to the generated .dll
        $<TARGET_FILE_DIR:AetherApp>                # The directory where the .exe is located
)

 # Copy .PDB in debug!
if (AETHER_DEBUG OR AETHER_RELEASE_DBG_INFO)
    add_custom_command(TARGET AetherApp POST_BUILD
       COMMAND ${CMAKE_COMMAND} -E copy                
            $<TARGET_PDB_FILE:AetherRenderer>
            $<TARGET_FILE_DIR:AetherApp>
    ) 
endif()

# Also copy both raw and compiled shaders!
get_property(cso_files GLOBAL PROPERTY CSO_FILES)
get_property(raw_shaders GLOBAL PROPERTY SHADER_FILES)

set(DEST_DIR "$<TARGET_FILE_DIR:AetherApp>/Shaders")

add_custom_command(TARGET AetherApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DEST_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${cso_files}               
        ${DEST_DIR}
        COMMAND_EXPAND_LISTS
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${raw_shaders}               
        ${DEST_DIR}
        COMMAND_EXPAND_LISTS
)