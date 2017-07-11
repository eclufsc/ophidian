file(GLOB ophidian_geometry_SRC
    "*.cpp"
)

add_library(ophidian_geometry ${ophidian_geometry_SRC})
target_include_directories(ophidian_geometry PUBLIC ${Boost_INCLUDE_DIRS})

install(TARGETS ophidian_geometry DESTINATION lib)
install(FILES Distance.h Models.h Operations.h DESTINATION include/ophidian/geometry)
