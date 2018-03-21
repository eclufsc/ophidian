find_path(LEF_INCLUDE_DIR "lefrReader.hpp" PATHS "/usr/include" )
find_library(LEF_LIBRARY "liblef.a" PATHS "/usr/lib" )

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(LEF DEFAULT_MSG
    LEF_INCLUDE_DIR
    LEF_LIBRARY
)

add_library(LEF::lef STATIC IMPORTED)
set_target_properties(LEF::lef PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${LEF_INCLUDE_DIR}
    IMPORTED_LOCATION ${LEF_LIBRARY}
)
