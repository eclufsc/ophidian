file(GLOB ophidian_timing_SRC
    "*.h"
    "*.cpp"
)
add_library(ophidian_timing ${ophidian_timing_SRC})
target_include_directories(ophidian_timing PUBLIC ${Boost_INCLUDE_DIRS})
install(TARGETS ophidian_timing DESTINATION lib)
install(FILES Graph.h DESTINATION include/ophidian/timing)
