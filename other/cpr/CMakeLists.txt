if(WIN32) # Install dlls in the same directory as the executable on Windows
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

include(FetchContent)
FetchContent_Declare(cpr
	GIT_REPOSITORY https://github.com/libcpr/cpr.git
	GIT_TAG beb9e98806bb84bcc130a2cebfbcbbc6ce62b335 	#v1.7.2
)
FetchContent_MakeAvailable(cpr)
