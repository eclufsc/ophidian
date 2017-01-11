cmake_minimum_required(VERSION 2.8.11)

project(ispd)

LINK_DIRECTORIES(${THIRD_PARTY_PATH}/LEF/lib/)
LINK_DIRECTORIES(${THIRD_PARTY_PATH}/DEF/lib/)

SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
SET(BUILD_SHARED_LIBRARIES OFF)
SET(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(ispd main.cpp)

target_link_libraries(ispd parsing standard_cell netlist placement interconnection)

add_custom_command(
        TARGET ispd POST_BUILD
        COMMAND ln -sf ${THIRD_PARTY_PATH} ${CMAKE_CURRENT_BINARY_DIR}
)
