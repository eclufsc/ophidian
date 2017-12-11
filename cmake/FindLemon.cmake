find_path(Lemon_INCLUDE_DIR "lemon/maps.h" PATHS "/usr/include" )
find_library(Lemon_LIBRARY "liblemon.a" "liblemon.so" NAMES "libemon.a" "libemon.so" PATHS "/usr/lib" "/usr/lib/x86_64-linux-gnu")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Lemon DEFAULT_MSG
    Lemon_INCLUDE_DIR
    Lemon_LIBRARY
)

add_library(Lemon::lemon STATIC IMPORTED)
set_target_properties(Lemon::lemon PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${Lemon_INCLUDE_DIR}
    IMPORTED_LOCATION ${Lemon_LIBRARY}
)
