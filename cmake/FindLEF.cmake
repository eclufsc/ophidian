find_path(LEF_INCLUDE_DIR
  NAMES
    "lefrReader.hpp"
)

find_library(LEF_LIBRARY
  NAMES
    "liblef.a"
    "liblef.so"
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(LEF
  FOUND_VAR
    LEF_FOUND
  REQUIRED_VARS
    LEF_LIBRARY
    LEF_INCLUDE_DIR
)

if(LEF_FOUND)
  set(Lemon_LIBRARIES ${Lemon_LIBRARY})
  set(Lemon_INCLUDE_DIRS ${Lemon_INCLUDE_DIR})
endif()

if(LEF_FOUND AND NOT TARGET LEF::def)
  add_library(LEF::lef UNKNOWN IMPORTED)
  set_target_properties(LEF::lef PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES ${LEF_INCLUDE_DIR}
      IMPORTED_LOCATION ${LEF_LIBRARY}
  )
endif()


mark_as_advanced(
  LEF_INCLUDE_DIR
  LEF_LIBRARY
)
