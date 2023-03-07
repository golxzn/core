cmake_minimum_required(VERSION 3.20)

set(root ${CMAKE_CURRENT_SOURCE_DIR})
include(${root}/cmake/prelude.cmake)

project(golxzn_core
	VERSION 0.2.4
	DESCRIPTION "[golxzn] basic stuff for the gxen game enging"
	HOMEPAGE_URL https://github.com/golxzn/core
	LANGUAGES CXX
)
include(${root}/cmake/options.cmake)

load_sources_list(headers ${GOLXZNC_INCLUDE_DIR} ".h;.hpp")
load_sources_list(sources ${GOLXZNC_SOURCES_DIR} ".cpp")

add_library(${target}_lib ${sources} ${headers})
add_library(golxzn::core ALIAS ${target}_lib)

include(GenerateExportHeader)
generate_export_header(${target}_lib
	BASE_NAME golxzn
	EXPORT_FILE_NAME export/golxzn/core/export.hpp
	CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)
if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(${target}_lib PRIVATE GOLXZN_STATIC_DEFINE)
endif()

set_target_properties(${target}_lib
	PROPERTIES
		CXX_STANDARD ${GOLXZNC_CXX_STANDARD}
		CXX_STANDARD_REQUIRED ON
		CXX_VISIBILITY_PRESET hidden
		VISIBILITY_INLINES_HIDDEN YES
		VERSION "${PROJECT_VERSION}"
		SOVERSION "${PROJECT_VERSION_MAJOR}"
		EXPORT_NAME ${GOLXZNC_PACKAGE_NAME}
		OUTPUT_NAME ${GOLXZNC_PACKAGE_NAME}
)


target_include_directories(${target}_lib ${warning_guard}
	PUBLIC
		"$<BUILD_INTERFACE:${GOLXZNC_INCLUDE_DIR}>"
		"$<INSTALL_INTERFACE:include>"
)
target_include_directories(${target}_lib SYSTEM
	PUBLIC
		"$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
		"$<INSTALL_INTERFACE:export>"
)

include(${GOLXZNC_DEPENDENCIED_DIR}/import.cmake)

target_link_libraries(${target}_lib PUBLIC ${libraries})
target_include_directories(${target}_lib PUBLIC ${includes})
target_compile_definitions(${target}_lib PUBLIC ${definitions})
target_compile_options(${target}_lib PRIVATE ${compile_options})

include(${root}/cmake/install/rules.cmake)
include(${root}/cmake/developer_mode.cmake)
