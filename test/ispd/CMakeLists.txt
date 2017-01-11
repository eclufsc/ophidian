cmake_minimum_required(VERSION 2.8.11)

project(ispd)

LINK_DIRECTORIES(${THIRD_PARTY_PATH}/LEF/lib/)
LINK_DIRECTORIES(${THIRD_PARTY_PATH}/DEF/lib/)


add_executable(ispd main.cpp)

target_link_libraries(ispd parsing standard_cell netlist placement interconnection)
