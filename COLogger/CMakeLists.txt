set(SOURCES
    OutputLogger.cpp
    OutputLogger.h
    pch.cpp
    pch.h
)

# --- Shared Library (DLL) ---
add_library(COLogger_shared SHARED ${SOURCES})

target_include_directories(COLogger_shared PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_precompile_headers(COLogger_shared PRIVATE pch.h)

target_compile_definitions(COLogger_shared PRIVATE DLL)

if(WIN32)
    target_compile_definitions(COLogger_shared PRIVATE _CONSOLE)
endif()

set_target_properties(COLogger_shared PROPERTIES OUTPUT_NAME "COLogger")

set(SHARED_OUTPUT_DIR "${OUTPUT_BASE}/$<CONFIG> - ${PLATFORM_NAME}/Shared")
set_target_properties(COLogger_shared PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${SHARED_OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${SHARED_OUTPUT_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${SHARED_OUTPUT_DIR}"
    PDB_NAME "COLogger"
    PDB_OUTPUT_DIRECTORY "${SHARED_OUTPUT_DIR}"
)


# --- Static Library ---
add_library(COLogger_static STATIC ${SOURCES})

target_include_directories(COLogger_static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_precompile_headers(COLogger_static PRIVATE pch.h)

if(WIN32)
    target_compile_definitions(COLogger_static PRIVATE _CONSOLE)
endif()

set_target_properties(COLogger_static PROPERTIES OUTPUT_NAME "COLogger")

set(STATIC_OUTPUT_DIR "${OUTPUT_BASE}/$<CONFIG> - ${PLATFORM_NAME}/Static")
set_target_properties(COLogger_static PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${STATIC_OUTPUT_DIR}"
    COMPILE_PDB_NAME "COLogger"
    COMPILE_PDB_OUTPUT_DIRECTORY "${STATIC_OUTPUT_DIR}"
)
