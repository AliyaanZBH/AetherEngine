# Try to locate DXGUID
find_library(DXGUID_LIBRARY dxguid
  HINTS
    $ENV{DXSDK_DIR}
    ${CMAKE_SYSTEM_LIBRARY_PATH}
    ${CMAKE_LIBRARY_PATH}
    [HINTS_FOR_VS_SDK_PATH]
)

# If we found the DXGUID library, create the DXGUID::DXGUID target
if (DXGUID_LIBRARY)
  add_library(DXGUID::DXGUID UNKNOWN IMPORTED)
  set_target_properties(DXGUID::DXGUID PROPERTIES
    IMPORTED_LOCATION "${DXGUID_LIBRARY}"
  )
  message(STATUS "Found DXGUID: ${DXGUID_LIBRARY}")
else ()
  message(FATAL_ERROR "Could not find dxguid")
endif ()
