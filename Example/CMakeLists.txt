set(EXAMPLE_TARGET FelisExample)
set(EXAMPLE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE EXAMPLE_SOURCES CONFIGURE_DEPENDS
    ${EXAMPLE_DIR}/*.cpp
    ${EXAMPLE_DIR}/*.h
)

add_executable(${EXAMPLE_TARGET} ${EXAMPLE_SOURCES})

target_include_directories(${EXAMPLE_TARGET} PRIVATE ${EXAMPLE_DIR})
target_compile_options(${EXAMPLE_TARGET} PRIVATE ${FELIS_WARNING_OPTIONS})
target_precompile_headers(${EXAMPLE_TARGET} PRIVATE ${EXAMPLE_DIR}/stdafx.h)

target_link_libraries(${EXAMPLE_TARGET} PRIVATE Felis::Framework)

set_target_properties(${EXAMPLE_TARGET}
	PROPERTIES
		CXX_EXTENSIONS OFF
)
