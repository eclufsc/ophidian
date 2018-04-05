find_path(LIBERTY_PARSER_INCLUDE_DIR "si2dr_liberty.h" PATHS "/usr/include" )
find_library(LIBERTY_PARSER_LIBRARY "libsi2dr_liberty.a" PATHS "/usr/lib" )

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Liberty DEFAULT_MSG
    LIBERTY_PARSER_INCLUDE_DIR
    LIBERTY_PARSER_LIBRARY
)

add_library(Liberty::parser STATIC IMPORTED)
set_target_properties(Liberty::parser PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${LIBERTY_PARSER_INCLUDE_DIR}
    IMPORTED_LOCATION ${LIBERTY_PARSER_LIBRARY}
)
