find_path(verilog-parser_INCLUDE_DIR "verilog_parser.h" PATHS "/usr/include")
find_library(verilog-parser_LIBRARY "libverilogparser.a" PATHS "/usr/lib")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(verilog-parser DEFAULT_MSG
    verilog-parser_INCLUDE_DIR
    verilog-parser_LIBRARY
)

add_library(verilog-parser::verilogparser STATIC IMPORTED)
set_target_properties(verilog-parser::verilogparser PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${verilog-parser_INCLUDE_DIR}
    IMPORTED_LOCATION ${verilog-parser_LIBRARY}
)
