project(ophidian)
cmake_minimum_required(VERSION 3.2.2)
set (CMAKE_CXX_STANDARD 14)

# Fetch Lemon and setup project to use Lemon as 3rdparty library
macro (UseLemon)
    include(${CMAKE_BINARY_DIR}/3rdparty/Lemon/cmake/LEMONConfig.cmake)
    include_directories(${CMAKE_BINARY_DIR}/3rdparty/Lemon/)
    include_directories(${CMAKE_SOURCE_DIR}/3rdparty/Lemon/)
endmacro()
#

execute_process(COMMAND git submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                RESULT_VARIABLE submodule_init_exit_code)

add_subdirectory(3rdparty)
link_directories(${CMAKE_BINARY_DIR}/3rdparty/verilog-parser/src/)

UseLemon()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fext-numeric-literals")
set(THIRD_PARTY_PATH ${CMAKE_SOURCE_DIR}/3rdparty)

include_directories(.)

enable_testing()
add_subdirectory(ophidian)
add_subdirectory(test)
