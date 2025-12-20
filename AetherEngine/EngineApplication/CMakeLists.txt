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
# Re-use Build Configuration macros from interface!
#
target_link_libraries(AetherApp PUBLIC AetherConfig)


# Add the deploy target! This ensures the command that copies the shader files happens every time!
add_dependencies(AetherApp deploy_shaders)

# Also explicitly add the engine as dependency - this ensures that when running the exe, the engine gets recompiled too :)
add_dependencies(AetherApp AetherRenderer)

# Link renderer to our app exe - use DLL for non-shipping builds to allow for hot-reloading
target_link_libraries(AetherApp PRIVATE AetherRenderer)
