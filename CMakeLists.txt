cmake_minimum_required(VERSION 3.25)

project(FelisFramework LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Default build type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

option(FELIS_BUILD_EXAMPLE "Build Felis example" OFF)
option(FELIS_BUILD_TEST "Build Felis test" OFF)

# Felis library
set(FELIS_TARGET FelisFramework)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/Source)

file(GLOB_RECURSE FELIS_SOURCES CONFIGURE_DEPENDS
    ${SOURCE_DIR}/*.h
    ${SOURCE_DIR}/*.cpp
)

add_library(${FELIS_TARGET} STATIC ${FELIS_SOURCES})

target_include_directories(${FELIS_TARGET} PRIVATE ${SOURCE_DIR})
target_compile_options(${FELIS_TARGET} PRIVATE -Wall -Wextra -Wpedantic)
target_precompile_headers(${FELIS_TARGET} PRIVATE ${SOURCE_DIR}/Felis/stdafx.h)

# Example
if(FELIS_BUILD_EXAMPLE)
    set(EXAMPLE_TARGET FelisExample)
    set(EXAMPLE_DIR ${CMAKE_SOURCE_DIR}/Example)

    file(GLOB_RECURSE EXAMPLE_SOURCES CONFIGURE_DEPENDS
        ${EXAMPLE_DIR}/*.cpp
        ${EXAMPLE_DIR}/*.h
    )

    add_executable(${EXAMPLE_TARGET} ${EXAMPLE_SOURCES})

    target_include_directories(${EXAMPLE_TARGET} PRIVATE ${EXAMPLE_DIR})
    target_compile_options(${EXAMPLE_TARGET} PRIVATE -Wall -Wextra -Wpedantic)
    target_precompile_headers(${EXAMPLE_TARGET} PRIVATE ${EXAMPLE_DIR}/stdafx.h)

    target_link_libraries(${EXAMPLE_TARGET} PRIVATE ${FELIS_TARGET})
    target_include_directories(${EXAMPLE_TARGET} PRIVATE ${SOURCE_DIR})
endif()

# Test
if(FELIS_BUILD_TEST)
    set(TEST_TARGET FelisTest)
    set(TEST_DIR ${CMAKE_SOURCE_DIR}/Test)

    file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
        ${TEST_DIR}/*.cpp
        ${TEST_DIR}/*.h
    )

    add_executable(${TEST_TARGET} ${TEST_SOURCES})

    target_include_directories(${TEST_TARGET} PRIVATE ${TEST_DIR})
    target_compile_options(${TEST_TARGET} PRIVATE -Wall -Wextra -Wpedantic)
    target_precompile_headers(${TEST_TARGET} PRIVATE ${TEST_DIR}/stdafx.h)

    target_link_libraries(${TEST_TARGET} PRIVATE ${FELIS_TARGET})
    target_include_directories(${TEST_TARGET} PRIVATE ${SOURCE_DIR})
endif()
