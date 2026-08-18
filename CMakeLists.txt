cmake_minimum_required(VERSION 3.25)

project(FelisFramework LANGUAGES CXX)

# Default build type
if(PROJECT_IS_TOP_LEVEL AND NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
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
