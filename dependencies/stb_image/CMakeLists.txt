cmake_minimum_required(VERSION 3.23)

project(stb_image
	VERSION 2.28.0
	DESCRIPTION "stb_image"
	LANGUAGES C
)

option(STB_IMAGE_INSTALL "Install stb_image" OFF)

add_library(stb_image STATIC src/stb_image.cpp include/stb_image.h)
add_library(stb::image ALIAS stb_image)

target_include_directories(stb_image SYSTEM PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

if(STB_IMAGE_INSTALL)

	include(CMakePackageConfigHelpers)
	include(GNUInstallDirs)

	install(TARGETS stb_image
		EXPORT stb_image_targets
		RUNTIME COMPONENT bin
		LIBRARY COMPONENT lib
		ARCHIVE COMPONENT lib
		PUBLIC_HEADER DESTINATION include COMPONENT lib
		INCLUDES DESTINATION include/
	)

	write_basic_package_version_file("$stb_imageConfigVersion.cmake" COMPATIBILITY SameMajorVersion)
	set(stb_image_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/stb_image
		CACHE PATH "CMake package config location relative to the install prefix"
	)

	install(EXPORT stb_image_targets
		NAMESPACE stb::
		DESTINATION ${stb_image_INSTALL_CMAKEDIR}
		COMPONENT stb_image_development
	)

endif()
