set(FELIS_TARGET FelisFramework)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE FELIS_SOURCES CONFIGURE_DEPENDS
    ${SOURCE_DIR}/*.h
    ${SOURCE_DIR}/*.cpp
)

add_library(${FELIS_TARGET} STATIC ${FELIS_SOURCES})
add_library(Felis::Framework ALIAS ${FELIS_TARGET})

target_include_directories(${FELIS_TARGET}
	PUBLIC
		$<BUILD_INTERFACE:${SOURCE_DIR}>
)

target_compile_features(${FELIS_TARGET} PUBLIC cxx_std_23)
target_compile_options(${FELIS_TARGET} PRIVATE ${FELIS_WARNING_OPTIONS})
target_precompile_headers(${FELIS_TARGET} PRIVATE ${SOURCE_DIR}/Felis/stdafx.h)

set_target_properties(${FELIS_TARGET}
	PROPERTIES
		CXX_EXTENSIONS OFF
		POSITION_INDEPENDENT_CODE ON
)
