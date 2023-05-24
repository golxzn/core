
set(ALLOW_DUPLICATE_CUSTOM_TARGETS ON)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

set(subdirs
	glm
	PlatformFolders
	filesystem
	stb_image
)

set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
set(PLATFORMFOLDERS_ENABLE_INSTALL ON CACHE BOOL "" FORCE)
set(GHC_FILESYSTEM_WITH_INSTALL ON CACHE BOOL "" FORCE)
set(STB_IMAGE_INSTALL ON CACHE BOOL "" FORCE)
foreach(subdir IN LISTS subdirs)
	add_subdirectory(${GOLXZNC_DEPENDENCIES_DIR}/${subdir} ${GOLXZNC_DEPENDENCIES_BUILD_DIR}/${subdir} EXCLUDE_FROM_ALL)
endforeach()

add_dependencies(${target}_lib
	glm::glm
	sago::platform_folders
	ghcFilesystem::ghc_filesystem
	stb::image
)
list(APPEND libraries
	glm::glm
	sago::platform_folders
	ghcFilesystem::ghc_filesystem
	stb::image
)
list(APPEND export_targets
	glm
)