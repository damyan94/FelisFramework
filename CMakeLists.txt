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

if(MSVC)
    set(FELIS_WARNING_OPTIONS /W4)
else()
    set(FELIS_WARNING_OPTIONS -Wall -Wextra -Wpedantic)
endif()

add_subdirectory(Source)

if(FELIS_BUILD_EXAMPLE)
    add_subdirectory(Example)
endif()

if(FELIS_BUILD_TEST)
    enable_testing()
    add_subdirectory(Test)
endif()
