# Figure out which backends we can use based on the current platform. Always include GLFW and OpenGL as they are cross-platform
set (IM_GUI_BACKENDS
    backends/imgui_impl_glfw.cpp
    backends/imgui_impl_opengl3.cpp
)

# If we're on windows, we can safely add the extra backends
if (WIN32)
     list(APPEND IM_GUI_BACKENDS
            backends/imgui_impl_win32.cpp
            backends/imgui_impl_dx11.cpp
            backends/imgui_impl_dx12.cpp
     )
endif()

# Create ImGui library with the core files and the necessary backends
add_library(imgui STATIC
    imgui.cpp
    imgui_draw.cpp
    imgui_widgets.cpp
    imgui_tables.cpp
    imgui_demo.cpp
    ${IM_GUI_BACKENDS}
)

# Set backend dir for includes
set(BACKEND_DIR ${CMAKE_CURRENT_SOURCE_DIR}/backends)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${BACKEND_DIR})

# Also link certain things for the backends!
target_link_libraries(imgui PUBLIC glfw glad)

