find_package( Boost 1.59 )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} ${THIRD_PARTY_PATH})

set(PLACEMENT_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/library.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cells.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lef.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/def.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/placement.cpp
)

add_subdirectory(legalization)
add_subdirectory(lef)

add_library (placement ${PLACEMENT_SOURCE} )

LINK_DIRECTORIES(${THIRD_PARTY_PATH}/LEF/lib/)
target_include_directories ( placement PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${THIRD_PARTY_PATH})


target_link_libraries( placement ${Boost_LIBRARIES} entity standard_cell netlist LEF )
