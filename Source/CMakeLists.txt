set(FELIS_TARGET FelisFramework)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE FELIS_SOURCES CONFIGURE_DEPENDS
    ${SOURCE_DIR}/*.h
    ${SOURCE_DIR}/*.cpp
)

add_library(${FELIS_TARGET} STATIC ${FELIS_SOURCES})

target_include_directories(${FELIS_TARGET} PUBLIC ${SOURCE_DIR})
target_compile_features(${FELIS_TARGET} PUBLIC cxx_std_23)
target_compile_options(${FELIS_TARGET} PRIVATE ${FELIS_WARNING_OPTIONS})
target_precompile_headers(${FELIS_TARGET} PRIVATE ${SOURCE_DIR}/Felis/stdafx.h)
