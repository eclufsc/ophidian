find_package( Boost 1.59 )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} ${THIRD_PARTY_PATH})

LINK_DIRECTORIES(${THIRD_PARTY_PATH}/LEF/lib/)
LINK_DIRECTORIES(${THIRD_PARTY_PATH}/DEF/lib/)

set(PARSING_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/lef.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/def.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/verilog.cpp
)

add_subdirectory(tau2014)

add_library (parsing ${PARSING_SOURCE} )

target_include_directories ( parsing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${THIRD_PARTY_PATH})
target_link_libraries( parsing ${Boost_LIBRARIES} LEF DEF )
