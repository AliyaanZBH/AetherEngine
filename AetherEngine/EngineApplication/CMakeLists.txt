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
    ${PROJECT_SOURCE_DIR}/RenderingEngine/Aether.h
)


# Define the executable
add_executable(AetherApp ${SRC} ${INCLUDE})

# Include directories for the executable
target_include_directories(AetherApp PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(AetherApp PRIVATE include)
target_include_directories(AetherApp PRIVATE include)

# Important step to ensure that the app IMPORTS the DLL instead of trying to export one!
#target_compile_definitions(AetherApp PRIVATE AETHER_BUILD_DLL=0)

# Set source groups for VS2022 quality of life
source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})

# Link renderer DLL to the app exe we are creating
target_link_libraries(AetherApp PRIVATE AetherRenderer)

# Post-build step to copy the DLL to the directory where the .exe is - huge time saver!
add_custom_command(TARGET AetherApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:AetherRenderer>               # The path to the generated .dll
        $<TARGET_FILE_DIR:AetherApp>                # The directory where the .exe is located
)

get_target_property(DEFINITIONS AetherApp COMPILE_DEFINITIONS)
message("AetherApp Compile Definitions: ${DEFINITIONS}")