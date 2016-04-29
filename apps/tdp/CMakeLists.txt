cmake_minimum_required(VERSION 2.8.11)

project(tdp)

LINK_DIRECTORIES(${THIRD_PARTY_PATH}/LEF/lib/)
LINK_DIRECTORIES(${THIRD_PARTY_PATH}/DEF/lib/)


add_executable(tdp main.cpp)

target_link_libraries(tdp timing-driven_placement)
