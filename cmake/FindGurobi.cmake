find_path(Gurobi_INCLUDE_DIR
  NAMES
    "gurobi_c++.h"
    "gurobi_c.h"
)

find_library(Gurobi_LIBRARY
  NAMES
    "libgurobi.so"
)

find_library(Gurobi_c++_LIBRARY
  NAMES
    "libgurobi_c++.a"
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Gurobi
  FOUND_VAR
    Gurobi_FOUND
  REQUIRED_VARS
    Gurobi_LIBRARY
    Gurobi_c++_LIBRARY
    Gurobi_INCLUDE_DIR
)

if(Gurobi_FOUND)
  set(Gurobi_LIBRARIES ${Gurobi_LIBRARY} ${Gurobi_c++_LIBRARY})
  set(Gurobi_INCLUDE_DIRS ${Gurobi_INCLUDE_DIR})
endif()

if(Gurobi_FOUND AND NOT TARGET Gurobi::gurobi)
  add_library(Gurobi::gurobi SHARED IMPORTED)
  set_target_properties(Gurobi::gurobi PROPERTIES
    IMPORTED_LOCATION "${Gurobi_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${Gurobi_INCLUDE_DIR}"
  )
endif()

if(Gurobi_FOUND AND NOT TARGET Gurobi::gurobi_c++)
  add_library(Gurobi::gurobi_c++ STATIC IMPORTED)
  set_target_properties(Gurobi::gurobi_c++ PROPERTIES
    IMPORTED_LOCATION "${Gurobi_c++_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${Gurobi_INCLUDE_DIR}"
  )
endif()

mark_as_advanced(
  Gurobi_INCLUDE_DIR
  Gurobi_LIBRARY
)

