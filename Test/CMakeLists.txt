set(TEST_TARGET FelisTest)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    ${TEST_DIR}/*.cpp
    ${TEST_DIR}/*.h
)

add_executable(${TEST_TARGET} ${TEST_SOURCES})

target_include_directories(${TEST_TARGET} PRIVATE ${TEST_DIR})
target_compile_options(${TEST_TARGET} PRIVATE ${FELIS_WARNING_OPTIONS})
target_precompile_headers(${TEST_TARGET} PRIVATE ${TEST_DIR}/stdafx.h)

target_link_libraries(${TEST_TARGET} PRIVATE FelisFramework)

if(NOT CMAKE_CROSSCOMPILING OR CMAKE_CROSSCOMPILING_EMULATOR)
    add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})
endif()
