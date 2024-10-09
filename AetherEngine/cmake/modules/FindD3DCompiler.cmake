# Try to locate D3DCompiler
find_path(D3DCOMPILER_INCLUDE_DIR d3dcompiler.h
  HINTS
    $ENV{DXSDK_DIR}
    ${CMAKE_SYSTEM_INCLUDE_PATH}
    ${CMAKE_INCLUDE_PATH}
    [HINTS_FOR_VS_SDK_PATH]
)

find_library(D3DCOMPILER_LIBRARY d3dcompiler
  HINTS
    $ENV{DXSDK_DIR}
    ${CMAKE_SYSTEM_LIBRARY_PATH}
    ${CMAKE_LIBRARY_PATH}
    [HINTS_FOR_VS_SDK_PATH]
)

# If we found the D3DCompiler include dir and library, create the D3DCompiler::D3DCompiler target
if (D3DCOMPILER_INCLUDE_DIR AND D3DCOMPILER_LIBRARY)
  add_library(D3DCompiler::D3DCompiler UNKNOWN IMPORTED)
  set_target_properties(D3DCompiler::D3DCompiler PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${D3DCOMPILER_INCLUDE_DIR}"
    IMPORTED_LOCATION "${D3DCOMPILER_LIBRARY}"
  )
  message(STATUS "Found D3DCompiler: ${D3DCOMPILER_LIBRARY}")
else ()
  message(FATAL_ERROR "Could not find D3DCompiler")
endif ()
