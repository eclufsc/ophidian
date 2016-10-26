project(ophidian)
cmake_minimum_required(VERSION 3.2.2)
set (CMAKE_CXX_STANDARD 14)

# set CMAKE_MODULE_PATH to pick up the additionals modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules/")

# Fetch Lemon and setup project to use Lemon as 3rdparty library
macro (UseLemon)
    include(${CMAKE_BINARY_DIR}/3rdparty/Lemon/cmake/LEMONConfig.cmake)
    include_directories(${CMAKE_BINARY_DIR}/3rdparty/Lemon/)
    include_directories(${CMAKE_SOURCE_DIR}/3rdparty/Lemon/)
    link_directories(${CMAKE_BINARY_DIR}/3rdparty/Lemon/lemon)
endmacro()
#

execute_process(COMMAND git submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                RESULT_VARIABLE submodule_init_exit_code)

add_subdirectory(3rdparty)
link_directories(${CMAKE_BINARY_DIR}/3rdparty/verilog-parser/src/)

UseLemon()
find_package(Boost 1.59 REQUIRED)

include(ExternalProject)
include_directories(.)

# Build DEF v5.8-p016 parser with the included Makefile
find_package(ZLIB REQUIRED)
ExternalProject_Add(
	def_parser
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/DEF
	# Empty CONFIGURE_COMMAND as the DEF parser has no configure file
	CONFIGURE_COMMAND ""
	BUILD_COMMAND make installhdrs installlib
	# DEF Makefile is not compatible with external builds
	BUILD_IN_SOURCE 1
)

# Build Liberty parser
find_package(GPERF REQUIRED)
#ExternalProject_Add(
#        liberty_parser
#        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libertyParser
#        CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libertyParser/configure --prefix=${CMAKE_BINARY_DIR}/3rdparty/libertyParser
#        PREFIX ${CMAKE_BINARY_DIR}/3rdparty/libertyParser
#        BUILD_COMMAND make install
#        BUILD_IN_SOURCE 1
#)
ExternalProject_Add(
        liberty_parser
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libertyParser
        CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libertyParser/configure --prefix=${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libertyParser
        PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libertyParser
        BUILD_COMMAND make
        BUILD_IN_SOURCE 1
)

set(THIRD_PARTY_PATH ${CMAKE_SOURCE_DIR}/3rdparty)
set(THIRD_PARTY_BUILD_PATH ${CMAKE_BINARY_DIR}/3rdparty)

enable_testing()
add_subdirectory(ophidian)
add_subdirectory(test)
