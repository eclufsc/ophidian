find_path(LibertyParser_INCLUDE_DIR "." PATHS "/usr/include" )
find_library(LibertyParser_shared_LIBRARY "libsi2dr_liberty.so" PATHS "/usr/lib" "/usr/lib/x86_64-linux-gnu")
find_library(LibertyParser_static_LIBRARY "libsi2dr_liberty.a"  PATHS "/usr/lib" "/usr/lib/x86_64-linux-gnu")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(LibertyParser DEFAULT_MSG
    LibertyParser_INCLUDE_DIR
    LibertyParser_shared_LIBRARY
    LibertyParser_static_LIBRARY
)

add_library(LibertyParser::LibertyParser SHARED IMPORTED)
set_target_properties(LibertyParser::LibertyParser PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES ${LibertyParser_INCLUDE_DIR}
	IMPORTED_LOCATION ${LibertyParser_shared_LIBRARY}
)

target_link_libraries(LibertyParser::LibertyParser)

add_library(LibertyParser::LibertyParser_static STATIC IMPORTED)
set_target_properties(LibertyParser::LibertyParser_static PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES ${LibertyParser_INCLUDE_DIR}
	IMPORTED_LOCATION ${LibertyParser_static_LIBRARY}
)

target_link_libraries(LibertyParser::LibertyParser_static)

