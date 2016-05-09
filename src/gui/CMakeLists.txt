# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 REQUIRED system window graphics network audio)

include_directories(${SFML_INCLUDE_DIR})

add_library (gui  canvas.cpp spatial_index.cpp drawable_batch.h)
target_include_directories (gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries( gui ${Boost_LIBRARIES} )
