cmake_minimum_required(VERSION 3.20)

project(golxzn_core_tests LANGUAGES CXX)

# ---- Dependencies ----
if(PROJECT_IS_TOP_LEVEL)
	find_package(GolxznCore REQUIRED)
	enable_testing()
endif()

CPMAddPackage("gh:catchorg/Catch2@${GOLXZNC_CATCH2_VERSION}")

# ---- Tests ----
load_sources_list(test_sources ${CMAKE_CURRENT_SOURCE_DIR}/sources ".h;.hpp;.cpp")
add_executable(${CMAKE_PROJECT_NAME}_exe ${test_sources})
target_link_libraries(${CMAKE_PROJECT_NAME}_exe PUBLIC
	golxzn::core
	Catch2::Catch2
	Catch2::Catch2WithMain
)
set_target_properties(${CMAKE_PROJECT_NAME}_exe PROPERTIES
	CXX_STANDARD ${GOLXZNC_CXX_STANDARD}
	CXX_STANDARD_REQUIRED ON
)

# ---- End-of-file commands ----
add_folders(Test)
