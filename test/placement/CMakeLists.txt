add_subdirectory(legalization)

set(SOURCE
   ${SOURCE}
   ${CMAKE_CURRENT_SOURCE_DIR}/cells_test.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/placement_test.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/def_test.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/lef_test.cpp
   PARENT_SCOPE
)
