#cmake_minimum_required(VERSION 2.8.11)

#project(sbcci2017)

#INCLUDE_DIRECTORIES( ${THIRD_PARTY_PATH} )
#LINK_DIRECTORIES( ${THIRD_PARTY_PATH}/DEF/lib/ ${THIRD_PARTY_PATH}/LEF/lib/ )

#SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
#SET(BUILD_SHARED_LIBRARIES OFF)
#SET(CMAKE_EXE_LINKER_FLAGS "-static")

#add_executable(sbcci2017 main.cpp)

#target_link_libraries(sbcci2017 ophidian_parser standard_cell netlist placement interconnection floorplan)

#add_custom_command(
#        TARGET sbcci2017 POST_BUILD
#        COMMAND ln -sf ${THIRD_PARTY_PATH} ${CMAKE_CURRENT_BINARY_DIR}
#)



aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)
set(SOURCE
   ${SOURCE}
   ${SRC_LIST}
   PARENT_SCOPE
)

