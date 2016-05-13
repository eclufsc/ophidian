add_library (routing grid_3d.cpp grid_3d.h metal_layers.cpp metal_layers.h)
target_include_directories ( routing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries( routing )
