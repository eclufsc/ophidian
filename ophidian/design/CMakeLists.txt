file(GLOB ophidian_design_SRC
    "*.cpp"
)

add_library(ophidian_design ${ophidian_design_SRC})
target_include_directories(ophidian_design PUBLIC ${THIRD_PARTY_SOURCE_DIR} )

install(TARGETS ophidian_design DESTINATION lib)
install(FILES Design.h DesignBuilder.h DESTINATION include/ophidian/design)
