cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(TARGET_NAME Application)
set(BASE_DIR ${CMAKE_SOURCE_DIR})
set(SOURCE_DIR ${BASE_DIR}/Source)

project(FelisFramework LANGUAGES CXX)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    ${SOURCE_DIR}/*.h
    ${SOURCE_DIR}/*.cpp
)

add_executable(${TARGET_NAME} ${SOURCE_FILES})

target_include_directories(${TARGET_NAME} PUBLIC ${SOURCE_DIR})
target_precompile_headers(${TARGET_NAME} PRIVATE ${SOURCE_DIR}/stdafx.h)
target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic)
