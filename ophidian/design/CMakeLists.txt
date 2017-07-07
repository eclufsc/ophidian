INCLUDE_DIRECTORIES( ${THIRD_PARTY_PATH} )

file(GLOB ophidian_design_SRC
    "*.h"
    "*.cpp"
)
add_library(ophidian_design ${ophidian_design_SRC})
install(TARGETS ophidian_design DESTINATION lib)
install(FILES Design.h DesignBuilder.h DESTINATION include/ophidian/design)
