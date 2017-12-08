find_path(Flute_INCLUDE_DIR "flute.h" PATHS "/usr/include")
find_library(Flute_LIBRARY "libflute.a" PATHS "/usr/lib")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Flute DEFAULT_MSG
    Flute_INCLUDE_DIR
    Flute_LIBRARY
)

add_library(Flute::flute STATIC IMPORTED)
set_target_properties(Flute::flute PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${Flute_INCLUDE_DIR}
    IMPORTED_LOCATION ${Flute_LIBRARY}
)
