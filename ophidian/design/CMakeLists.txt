file(GLOB ophidian_design_SRC
    "*.h"
    "*.cpp"
)
add_library(ophidian_design ${ophidian_design_SRC})
install(TARGETS ophidian_design DESTINATION lib)
install(FILES Design.h DESTINATION include/ophidian/design)
