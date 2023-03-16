
set(ALLOW_DUPLICATE_CUSTOM_TARGETS ON)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

if(GOLXZNC_USE_BOOST)
	CPMAddPackage(NAME Boost
		VERSION ${GOLXZNC_BOOST_VERSION}
		GITHUB_REPOSITORY "boostorg/boost"
		GIT_TAG "boost-${GOLXZNC_BOOST_VERSION}"
	)
	list(APPEND libraries ${Boost_LIBRARIES})
	list(APPEND includes ${Boost_INCLUDE_DIR})
endif()

set(SPDLOG_INSTALL ON)
add_subdirectory(${GOLXZNC_DEPENDENCIES_DIR}/spdlog ${GOLXZNC_DEPENDENCIES_BUILD_DIR}/spdlog EXCLUDE_FROM_ALL)

set(PLATFORMFOLDERS_ENABLE_INSTALL ON)
add_subdirectory(${GOLXZNC_DEPENDENCIES_DIR}/PlatformFolders ${GOLXZNC_DEPENDENCIES_BUILD_DIR}/PlatformFolders EXCLUDE_FROM_ALL)

set(GHC_FILESYSTEM_WITH_INSTALL ON)
add_subdirectory(${GOLXZNC_DEPENDENCIES_DIR}/filesystem EXCLUDE_FROM_ALL)

add_dependencies(${target}_lib
	spdlog::spdlog
	sago::platform_folders
	ghcFilesystem::ghc_filesystem
)
list(APPEND libraries
	spdlog::spdlog
	sago::platform_folders
	ghcFilesystem::ghc_filesystem
)
