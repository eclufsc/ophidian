cmake_minimum_required(VERSION 2.8.11)

project(avg_placement)

add_executable(avg_placement main.cpp)

target_link_libraries(avg_placement entity placement netlist standard_cell)
