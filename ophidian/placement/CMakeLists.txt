INCLUDE_DIRECTORIES( ${THIRD_PARTY_PATH} )

file(GLOB ophidian_placement_SRC
    "*.h"
    "*.cpp"
)
add_library(ophidian_placement ${ophidian_placement_SRC})
install(TARGETS ophidian_placement DESTINATION lib)
install(FILES Placement.h Library.h DESTINATION include/ophidian/placement)
