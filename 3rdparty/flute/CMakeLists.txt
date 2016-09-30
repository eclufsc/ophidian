add_library(flute STATIC flute_new.cpp flute.h)
install(TARGETS flute DESTINATION lib)
install(FILES flute.h DESTINATION include/ophidian/3rdparty/flute)
