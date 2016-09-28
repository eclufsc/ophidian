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
find_package(Boost 1.59 REQUIRED)

# Build LEF v5.8-p016 parser with the included Makefile
include(ExternalProject)
ExternalProject_Add(
	lef_parser
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/LEF
	# Empty CONFIGURE_COMMAND as the LEF parser has no configure file
	CONFIGURE_COMMAND ""
	BUILD_COMMAND make installhdrs installlib
	# LEF Makefile is not compatible with external builds
	BUILD_IN_SOURCE 1
)

if (CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fext-numeric-literals")
endif()

set(THIRD_PARTY_PATH ${CMAKE_SOURCE_DIR}/3rdparty)

include_directories(.)

enable_testing()
add_subdirectory(ophidian)
add_subdirectory(test)
