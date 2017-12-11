find_path(Lemon_INCLUDE_DIR "lemon/maps.h" PATHS "/usr/include" )
find_library(Lemon_shared_LIBRARY "liblemon.so" NAMES "libemon.so" PATHS "/usr/lib" "/usr/lib/x86_64-linux-gnu")
find_library(Lemon_static_LIBRARY "liblemon.a" NAMES "libemon.a" PATHS "/usr/lib" "/usr/lib/x86_64-linux-gnu")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Lemon DEFAULT_MSG
    Lemon_INCLUDE_DIR
    Lemon_shared_LIBRARY
    Lemon_static_LIBRARY
)

add_library(Lemon::lemon SHARED IMPORTED)
set_target_properties(Lemon::lemon PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${Lemon_INCLUDE_DIR}
    IMPORTED_LOCATION ${Lemon_shared_LIBRARY}
)

add_library(Lemon::lemon_static STATIC IMPORTED)
set_target_properties(Lemon::lemon_static PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${Lemon_INCLUDE_DIR}
    IMPORTED_LOCATION ${Lemon_static_LIBRARY}
)
