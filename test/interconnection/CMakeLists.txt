aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)
set(SOURCE
   ${SOURCE}
   ${SRC_LIST}
   PARENT_SCOPE
)

execute_process(COMMAND ln -f -s ${CMAKE_SOURCE_DIR}/3rdparty/flute/PORT9.dat ${CMAKE_BINARY_DIR}/test/.
                RESULT_VARIABLE result )
execute_process(COMMAND ln -f -s ${CMAKE_SOURCE_DIR}/3rdparty/flute/POWV9.dat ${CMAKE_BINARY_DIR}/test/.
                RESULT_VARIABLE result )
