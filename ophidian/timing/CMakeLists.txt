file(GLOB ophidian_timing_SRC
    "*.cpp"
)

add_library(ophidian_timing ${ophidian_timing_SRC})
target_include_directories(ophidian_timing PUBLIC ${THIRD_PARTY_SOURCE_DIR}) 
target_link_libraries(ophidian_timing PUBLIC OpenTimer)

install(TARGETS ophidian_timing DESTINATION lib)
install(FILES OpenTimer.h DESTINATION include/ophidian/timer)
