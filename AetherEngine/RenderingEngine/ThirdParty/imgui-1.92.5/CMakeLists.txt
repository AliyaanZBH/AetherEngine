# Begin with core ImGui target
add_library(imgui STATIC
    imgui.cpp
    imgui_draw.cpp
    imgui_widgets.cpp
    imgui_tables.cpp
    imgui_demo.cpp
)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Now build targets for each backend we want to implement

add_library(imgui_backend_glfw STATIC
    backends/imgui_impl_glfw.cpp
)
target_include_directories(imgui_backend_glfw PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(imgui_backend_glfw PUBLIC imgui glfw)

add_library(imgui_backend_gl3 STATIC
    backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui_backend_gl3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(imgui_backend_gl3 PUBLIC imgui)


add_library(imgui_backend_win32 STATIC
    backends/imgui_impl_win32.cpp
)
target_include_directories(imgui_backend_win32 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(imgui_backend_win32 PUBLIC imgui)

add_library(imgui_backend_dx11 STATIC
    backends/imgui_impl_dx11.cpp
)
target_include_directories(imgui_backend_dx11 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(imgui_backend_dx11 PUBLIC imgui d3d11 dxgi)
