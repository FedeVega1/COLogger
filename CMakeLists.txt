cmake_minimum_required(VERSION 3.20)

project(COLogger VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_GENERATOR_PLATFORM)
    set(PLATFORM_NAME ${CMAKE_GENERATOR_PLATFORM})
else()
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(PLATFORM_NAME "x64")
    else()
        set(PLATFORM_NAME "x86")
    endif()
endif()

set(OUTPUT_BASE "${CMAKE_SOURCE_DIR}/bin")

add_subdirectory(COLogger)
