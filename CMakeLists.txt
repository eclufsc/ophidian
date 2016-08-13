project(ophidian)
cmake_minimum_required(VERSION 3.5.1)

set (CMAKE_CXX_STANDARD 14)

find_package( Boost 1.59 REQUIRED )
include_directories( ${Boost_INCLUDE_DIRS} )

enable_testing()

add_subdirectory(ophidian)
add_subdirectory(test)
