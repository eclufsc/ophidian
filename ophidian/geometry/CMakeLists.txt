file(GLOB ophidian_geometry_SRC
    "*.h"
    "*.cpp"
)
add_library(ophidian_geometry ${ophidian_geometry_SRC})
target_include_directories(ophidian_geometry PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(ophidian_geometry PUBLIC )
target_include_directories(ophidian_geometry PUBLIC ${Boost_INCLUDE_DIRS})
install(TARGETS ophidian_geometry DESTINATION lib)
install(FILES Distance.h Models.h Operations.h DESTINATION include/ophidian/geometry)
