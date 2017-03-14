project(ophidian)
cmake_minimum_required(VERSION 3.2.2)
set (CMAKE_CXX_STANDARD 14)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

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

# Build LEF v5.8-p016 parser with the included Makefile
include(ExternalProject)
include_directories(.)

set(THIRD_PARTY_PATH ${CMAKE_SOURCE_DIR}/3rdparty)

ExternalProject_Add(
	lef_parser
        SOURCE_DIR ${THIRD_PARTY_PATH}/LEF
	# Empty CONFIGURE_COMMAND as the LEF parser has no configure file
	CONFIGURE_COMMAND ""
	BUILD_COMMAND make installhdrs installlib
	# LEF Makefile is not compatible with external builds
	BUILD_IN_SOURCE 1
)

# Build DEF v5.8-p016 parser with the included Makefile
ExternalProject_Add(
	def_parser
    SOURCE_DIR ${THIRD_PARTY_PATH}/DEF
	# Empty CONFIGURE_COMMAND as the DEF parser has no configure file
	CONFIGURE_COMMAND ""
	BUILD_COMMAND make installhdrs installlib
	# DEF Makefile is not compatible with external builds
	BUILD_IN_SOURCE 1
)

if (CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fext-numeric-literals")
endif()


enable_testing()
add_subdirectory(ophidian)
add_subdirectory(test)
