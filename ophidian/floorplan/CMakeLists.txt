file(GLOB ophidian_floorplan_SRC
    "*.cpp"
)

add_library(ophidian_floorplan ${ophidian_floorplan_SRC})
target_include_directories(ophidian_floorplan PUBLIC ${THIRD_PARTY_SOURCE_DIR})

install(TARGETS ophidian_floorplan DESTINATION lib)
install(FILES Floorplan.h DESTINATION include/ophidian/floorplan)
