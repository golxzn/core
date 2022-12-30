
if(GOLXZNC_USE_BOOST)
	CPMAddPackage(NAME Boost
		VERSION ${GOLXZNC_BOOST_VERSION}
		GITHUB_REPOSITORY "boostorg/boost"
		GIT_TAG "boost-${GOLXZNC_BOOST_VERSION}"
	)
	list(APPEND libraries ${Boost_LIBRARIES})
	list(APPEND includes ${Boost_INCLUDE_DIR})
endif()
