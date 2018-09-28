find_package(Threads REQUIRED)

find_path(OpenTimer_INCLUDE_DIR "ot" PATHS "/usr/include" )
find_library(OpenTimer_shared_LIBRARY "libOpenTimer.so" PATHS "/usr/lib" "/usr/lib/x86_64-linux-gnu")
find_library(OpenTimer_static_LIBRARY "libOpenTimer.a"  PATHS "/usr/lib" "/usr/lib/x86_64-linux-gnu")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(OpenTimer DEFAULT_MSG
    OpenTimer_INCLUDE_DIR
    OpenTimer_shared_LIBRARY
    OpenTimer_static_LIBRARY
)

add_library(OpenTimer::OpenTimer SHARED IMPORTED)
set_target_properties(OpenTimer::OpenTimer PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${OpenTimer_INCLUDE_DIR}
    IMPORTED_LOCATION ${OpenTimer_shared_LIBRARY}
)

target_link_libraries(OpenTimer::OpenTimer 
    INTERFACE Threads::Threads
    INTERFACE stdc++fs
)

add_library(OpenTimer::OpenTimer_static STATIC IMPORTED)
set_target_properties(OpenTimer::OpenTimer_static PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${OpenTimer_INCLUDE_DIR}
    IMPORTED_LOCATION ${OpenTimer_static_LIBRARY}
)

target_link_libraries(OpenTimer::OpenTimer_static 
    INTERFACE Threads::Threads
    INTERFACE stdc++fs
)
