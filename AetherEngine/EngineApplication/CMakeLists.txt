#
# Main application for Aether
#

# Include directories for source code
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/RenderingEngine/)


set(SRC
    App.cpp
)

set(INCLUDE
    ${PROJECT_SOURCE_DIR}/RenderingEngine/Core/include/Aether.h
)


# Define the executable
add_executable(AetherApp ${SRC} ${INCLUDE})

# Include directories for the executable
target_include_directories(AetherApp PRIVATE 
    ${PROJECT_SOURCE_DIR}/include
    include
)

# Set source groups for VS2022 quality of life
source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})


#
# Build Configuration specific options
#


# Configuration compile definitions
target_compile_definitions(AetherApp PRIVATE
    $<CONFIG:Debug> AETHER_DEBUG
    $<CONFIG:Release> AETHER_RELEASE
    $<CONFIG:RelWithDebInfo> AETHER_RELEASE_DBG_INFO
    $<CONFIG:Distribution> AETHER_DIST
)

# Also explicitly add the DLL as dependency - this ensures that when running the exe, the DLL gets recompiled too :)
add_dependencies(AetherApp AetherRenderer)
# Also add the deploy target! This ensures the command that copies the files happens every time!
add_dependencies(AetherApp deploy_shaders)

# Link renderer DLL to our app exe
target_link_libraries(AetherApp PRIVATE AetherRenderer)
