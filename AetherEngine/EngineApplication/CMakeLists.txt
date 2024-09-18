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
target_compile_definitions(AetherApp PRIVATE $<CONFIG:Distribution> AETHER_DIST)

# Enable optimisations when not in debug
if (AETHER_RELEASE OR AETHER_DIST)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()


# Link renderer DLL to the app exe we are creating
target_link_libraries(AetherApp PRIVATE AetherRenderer)

# Post-build step to copy the DLL to the directory where the .exe is - huge time saver!
add_custom_command(TARGET AetherApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:AetherRenderer>               # The path to the generated .dll
        $<TARGET_FILE_DIR:AetherApp>                # The directory where the .exe is located
)