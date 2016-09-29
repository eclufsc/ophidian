file(GLOB ophidian_geometry_SRC
    "*.h"
    "*.cpp"
)
include_directories(${Boost_INCLUDE_DIRS})
add_library(ophidian_geometry ${ophidian_geometry_SRC})
target_link_libraries(ophidian_geometry PUBLIC )
install(TARGETS ophidian_geometry DESTINATION lib)
install(FILES Distance.h Models.h Operations.h DESTINATION include/ophidian/geometry)
