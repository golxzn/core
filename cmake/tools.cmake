
function(load_sources_list result path extensions)
	set(sources "")
	foreach(ext IN LISTS extensions)
		file(GLOB_RECURSE additional CONFIGURE_DEPENDS ${path}/*${ext})
		list(APPEND sources ${additional})
	endforeach()

	list(REMOVE_DUPLICATES sources)
	source_group(TREE ${path} FILES ${sources})

	set(${result} ${sources} PARENT_SCOPE)
endfunction()

# Call this function at the end of a directory scope to assign a folder to
# targets created in that directory. Utility targets will be assigned to the
# UtilityTargets folder, otherwise to the ${name}Targets folder. If a target
# already has a folder assigned, then that target will be skipped.
function(add_folders name)
	if (NOT __USE_FOLDERS_INITIALIZED)
		set_property(GLOBAL PROPERTY USE_FOLDERS YES)
		set(__USE_FOLDERS_INITIALIZED ON)
	endif()

	get_property(targets DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)
	foreach(target IN LISTS targets)
		get_property(folder TARGET "${target}" PROPERTY FOLDER)
		if(DEFINED folder)
			continue()
		endif()
		set(folder Utility)
		get_property(type TARGET "${target}" PROPERTY TYPE)
		if(NOT type STREQUAL "UTILITY")
			set(folder "${name}")
		endif()
		set_property(TARGET "${target}" PROPERTY FOLDER "${folder}Targets")
	endforeach()
endfunction()

include(CMakeParseArguments)

function(get_cpm)
	set(version VERSION RENAME)
	cmake_parse_arguments(GET_CPM "" "${version}" "" ${ARGN})
	if(NOT GET_CPM_VERSION)
		message(FATAL_ERROR "[get_cpm]: Version was not found! Set VERSION parameter explicitly!")
	endif()

	if(CPM_SOURCE_CACHE)
		set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM_${GET_CPM_VERSION}.cmake")
	elseif(DEFINED ENV{CPM_SOURCE_CACHE})
		set(CPM_DOWNLOAD_LOCATION "$ENV{CPM_SOURCE_CACHE}/cpm/CPM_${GET_CPM_VERSION}.cmake")
	else()
		set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${GET_CPM_VERSION}.cmake")
	endif()

	if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
		message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
		file(DOWNLOAD
			https://github.com/TheLartians/CPM.cmake/releases/download/v${GET_CPM_VERSION}/CPM.cmake
			${CPM_DOWNLOAD_LOCATION}
		)
	endif()

	include(${CPM_DOWNLOAD_LOCATION})
endfunction()

macro(get_param_name name)
	string(FIND ${param} " " __param_name_sep)
	string(SUBSTRING ${param} 0 ${__param_name_sep} __param_name)
	set(${name} ${__param_name})
	unset(__param_name_sep)
	unset(__param_name)
endmacro()

function(add_subdirectory_params directory)
	foreach(param IN LISTS ARGN)
		get_param_name(param_name)
		set(${param_name}_value ${param_name})
		set(${param})
	endforeach()

	add_subdirectory(${directory} ${ADD_SUBDIR_PAR_bin_dir} ${ADD_SUBDIR_PAR_exclude})

	foreach(param IN LISTS ARGN)
		get_param_name(param_name)
		set(${param_name} ${${param_name}_value})
	endforeach()
endfunction()
