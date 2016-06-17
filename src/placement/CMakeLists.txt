find_package( Boost 1.59 )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} ${THIRD_PARTY_PATH})

set(PLACEMENT_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/hpwl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/library.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cells.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/placement.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lef2library.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/def2placement.cpp
)

add_library (placement ${PLACEMENT_SOURCE} )

LINK_DIRECTORIES(${THIRD_PARTY_PATH}/LEF/lib/)
LINK_DIRECTORIES(${THIRD_PARTY_PATH}/DEF/lib/)

target_include_directories ( placement PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${THIRD_PARTY_PATH})


target_link_libraries( placement ${Boost_LIBRARIES} entity_system standard_cell netlist interconnection LEF DEF )
