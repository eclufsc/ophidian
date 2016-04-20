cmake_minimum_required(VERSION 2.8.11)

project(interconnect_delay)

add_executable(interconnect_delay main.cpp)

target_link_libraries(interconnect_delay timing interconnection standard_cell)
