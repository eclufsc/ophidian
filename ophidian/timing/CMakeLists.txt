INCLUDE_DIRECTORIES( ${THIRD_PARTY_PATH} )

file(GLOB ophidian_timing_SRC
    "*.h"
    "*.cpp"
)

add_library(ophidian_timing ${ophidian_timing_SRC})

target_link_libraries(ophidian_timing PUBLIC OpenTimer)

install(TARGETS ophidian_timing DESTINATION lib)
install(FILES OpenTimer.h DESTINATION include/ophidian/timer)
