function(uncrustify_check configfile filelist)
    message("Running Uncrustify_check function")
    foreach(file ${filelist})
        execute_process(
            COMMAND uncrustify -c ${configfile} --check ${filelist}
            RESULT_VARIABLE uncrustify_it_result
        )
        if(uncrustify_it_result)
            message( FATAL_ERROR "uncrustify_check error on ${file}")
        endif()
    endforeach()
endfunction()

function(uncrustify_it configfile filelist)
    message("Running Uncrustify_it function")
    foreach(file ${filelist})
        execute_process(
            COMMAND uncrustify -c ${configfile} --replace --no-backup ${file}
            RESULT_VARIABLE uncrustify_it_result
        )
        if(uncrustify_it_result)
            message( FATAL_ERROR "uncrustify_it error on ${file}")
        endif()
    endforeach()
endfunction()
