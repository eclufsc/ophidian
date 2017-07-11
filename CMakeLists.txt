cmake_minimum_required(VERSION 3.2.2)

set(PROJECT_NAME "ophidian")
project(${PROJECT_NAME})

set(THIRD_PARTY_SOURCE_DIR ${PROJECT_SOURCE_DIR}/3rdparty)

set(CMAKE_CXX_STANDARD 14)

execute_process(
    COMMAND git submodule update --init --recursive
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    RESULT_VARIABLE submodule_init_exit_code
)

find_package(Boost 1.59 REQUIRED)
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fext-numeric-literals")
endif()

option(BUILD_SHARED_LIBS "on/off -> (build every non statc lib shared)/(build everything static)" ON)

include_directories(${PROJECT_SOURCE_DIR})

enable_testing()
add_subdirectory(3rdparty)

# Fetch Lemon and setup project to use Lemon as 3rdparty library
include(${CMAKE_BINARY_DIR}/3rdparty/Lemon/cmake/LEMONConfig.cmake)
include_directories(${CMAKE_BINARY_DIR}/3rdparty/Lemon/)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/Lemon/)
link_directories(${CMAKE_BINARY_DIR}/3rdparty/Lemon/lemon)

add_subdirectory(ophidian)
add_subdirectory(test)
