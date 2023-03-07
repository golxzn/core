if(CMAKE_SKIP_INSTALL_RULES)
	return()
endif()

if(PROJECT_IS_TOP_LEVEL)
	set(CMAKE_INSTALL_INCLUDEDIR include/ CACHE PATH "")
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package ${GOLXZNC_PACKAGE_NAME})

install(DIRECTORY "${GOLXZNC_INCLUDE_DIR}/" "${PROJECT_BINARY_DIR}/export/"
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
	COMPONENT ${package}_Development
)

install(TARGETS ${target}_lib
	EXPORT ${package}Targets
	RUNTIME COMPONENT ${package}_Runtime
	LIBRARY COMPONENT ${package}_Runtime
	NAMELINK_COMPONENT ${package}_Development
	ARCHIVE COMPONENT ${package}_Development
	PUBLIC_HEADER DESTINATION include COMPONENT ${package}_Development
	INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file("${package}ConfigVersion.cmake" COMPATIBILITY SameMajorVersion)

# Allow package maintainers to freely override the path for the configs
set(${package}_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
	CACHE PATH "CMake package config location relative to the install prefix"
)
mark_as_advanced(${package}_INSTALL_CMAKEDIR)

install(FILES ${root}/cmake/install/config.cmake
	DESTINATION "${${package}_INSTALL_CMAKEDIR}"
	RENAME "${package}Config.cmake"
	COMPONENT ${package}_Development
)

install(FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
	DESTINATION "${${package}_INSTALL_CMAKEDIR}"
	COMPONENT ${package}_Development
)

install(EXPORT ${package}Targets
	NAMESPACE golxzn::
	DESTINATION "${${package}_INSTALL_CMAKEDIR}"
	COMPONENT ${package}_Development
)

if(PROJECT_IS_TOP_LEVEL)
	include(CPack)
endif()
unset(package)
