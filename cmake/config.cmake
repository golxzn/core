
get_cpm(VERSION ${GOLXZNC_CPM_VERSION})

if(GOLXZNC_USE_BOOST)
	list(APPEND definitions GOLXZN_BOOSTED)
endif()

list(APPEND definitions $<$<CONFIG:Debug,RelWithDebInfo,UnityDebug>:GOLXZNC_DEBUG>)

# Allow disabling exceptions, but warn the user about the consequences
if(NOT GOLXZNC_ENABLE_EXCEPTIONS)
	message(WARNING "Exceptions have been disabled. Any operation that would "
					"throw an exception will result in a call to std::abort() instead.")
	list(APPEND definitions GOLXZNC_NO_EXCEPTIONS)
	if (MSVC)
		list(APPEND compile_options /EHs-c-)
	else()
		list(APPEND compile_options -fno-exceptions)
	endif()
endif()

list(APPEND definitions _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
