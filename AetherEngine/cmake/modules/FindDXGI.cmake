# Try to locate DXGI
find_path(DXGI_INCLUDE_DIR dxgi.h
  HINTS
    $ENV{DXSDK_DIR}
    ${CMAKE_SYSTEM_INCLUDE_PATH}
    ${CMAKE_INCLUDE_PATH}
    [HINTS_FOR_VS_SDK_PATH]
)

find_library(DXGI_LIBRARY dxgi
  HINTS
    $ENV{DXSDK_DIR}
    ${CMAKE_SYSTEM_LIBRARY_PATH}
    ${CMAKE_LIBRARY_PATH}
    [HINTS_FOR_VS_SDK_PATH]
)

# If we found the DXGI include dir and library, create the DXGI::DXGI target
if (DXGI_INCLUDE_DIR AND DXGI_LIBRARY)
  add_library(DXGI::DXGI UNKNOWN IMPORTED)
  set_target_properties(DXGI::DXGI PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${DXGI_INCLUDE_DIR}"
    IMPORTED_LOCATION "${DXGI_LIBRARY}"
  )
  message(STATUS "Found DXGI: ${DXGI_LIBRARY}")
else ()
  message(FATAL_ERROR "Could not find DXGI")
endif ()
