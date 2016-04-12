cmake_minimum_required(VERSION 2.8.11)

project(tdp)

add_executable(tdp main.cpp)

target_link_libraries(tdp timing-driven_placement)
