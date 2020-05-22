find_path(DEF_INCLUDE_DIR
  NAMES
    "defrReader.hpp"
)

find_library(DEF_LIBRARY
  NAMES
    "libdef.a"
    "libdef.so"
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(DEF
  FOUND_VAR
    DEF_FOUND
  REQUIRED_VARS
    DEF_LIBRARY
    DEF_INCLUDE_DIR
)

if(DEF_FOUND)
  set(Lemon_LIBRARIES ${Lemon_LIBRARY})
  set(Lemon_INCLUDE_DIRS ${Lemon_INCLUDE_DIR})
endif()

if(DEF_FOUND AND NOT TARGET DEF::def)
  add_library(DEF::def UNKNOWN IMPORTED)
  set_target_properties(DEF::def PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES ${DEF_INCLUDE_DIR}
      IMPORTED_LOCATION ${DEF_LIBRARY}
  )
endif()


mark_as_advanced(
  DEF_INCLUDE_DIR
  DEF_LIBRARY
)
