
get_cpm(VERSION ${GOLXZNC_CPM_VERSION})

if(GOLXZNC_USE_BOOST)
	list(APPEND definitions GOLXZN_BOOSTED)
endif()

list(APPEND definitions $<$<CONFIG:Debug,RelWithDebInfo,UnityDebug>:GOLXZN_DEBUG>)

