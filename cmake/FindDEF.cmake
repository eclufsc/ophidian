find_path(DEF_INCLUDE_DIR "defrReader.hpp" PATHS "/usr/include" )
find_library(DEF_LIBRARY "libdef.a" PATHS "/usr/lib" )

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(DEF DEFAULT_MSG
    DEF_INCLUDE_DIR
    DEF_LIBRARY
)

add_library(DEF::def STATIC IMPORTED)
set_target_properties(DEF::def PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${DEF_INCLUDE_DIR}
    IMPORTED_LOCATION ${DEF_LIBRARY}
)
