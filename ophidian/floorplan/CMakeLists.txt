INCLUDE_DIRECTORIES( ${THIRD_PARTY_PATH} )

file(GLOB ophidian_floorplan_SRC
    "*.h"
    "*.cpp"
)
add_library(ophidian_floorplan ${ophidian_floorplan_SRC})
install(TARGETS ophidian_floorplan DESTINATION lib)
install(FILES Floorplan.h DESTINATION include/ophidian/floorplan)
