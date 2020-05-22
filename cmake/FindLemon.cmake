find_package(LEMON QUIET)

find_path(Lemon_INCLUDE_DIR
  NAMES
    "lemon/maps.h"
  PATHS
    ${LEMON_INCLUDE_DIR}
)

find_library(Lemon_LIBRARY
  NAMES
    "libemon.a"
    "libemon.so"
    "liblemon.a"
    "liblemon.so"
    "lemon.lib"
  PATHS
    ${LEMON_LIBRARY_DIR}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Lemon
  FOUND_VAR
    Lemon_FOUND
  REQUIRED_VARS
    Lemon_LIBRARY
    Lemon_INCLUDE_DIR
)

if(Lemon_FOUND)
  set(Lemon_LIBRARIES ${Lemon_LIBRARY})
  set(Lemon_INCLUDE_DIRS ${Lemon_INCLUDE_DIR})
endif()

if(Lemon_FOUND AND NOT TARGET Lemon::lemon)
  add_library(Lemon::lemon UNKNOWN IMPORTED)
  set_target_properties(Lemon::lemon PROPERTIES
    IMPORTED_LOCATION "${Lemon_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${Lemon_INCLUDE_DIR}"
  )
endif()

mark_as_advanced(
  Lemon_INCLUDE_DIR
  Lemon_LIBRARY
)
