#ifndef OPHIDIAN_TEST_PARSER_VERILOG_TEST_H
#define OPHIDIAN_TEST_PARSER_VERILOG_TEST_H

#include <sstream>

namespace test
{

const std::string simpleInput = "module simple (\n"
                                "inp1,\n"
                                "inp2,\n"
                                "iccad_clk,\n"
                                "out\n"
                                ");\n"
                                "// Start PIs\n"
                                "input inp1;\n"
                                "input inp2;\n"
                                "input iccad_clk;\n"
                                "// Start POs\n"
                                "output out;\n"
                                "// Start wires\n"
                                "wire n1;\n"
                                "wire n2;\n"
                                "wire n3;\n"
                                "wire n4;\n"
                                "wire inp1;\n"
                                "wire inp2;\n"
                                "wire iccad_clk;\n"
                                "wire out;\n"
                                "wire lcb1_fo;\n"
                                "// Start cells\n"
                                "NAND2_X1 u1 ( .a(inp1), .b(inp2), .o(n1) );\n"
                                "NOR2_X1 u2 ( .a(n1), .b(n3), .o(n2) );\n"
                                "DFF_X80 f1 ( .d(n2), .ck(lcb1_fo), .q(n3) );\n"
                                "INV_X1 u3 ( .a(n3), .o(n4) );\n"
                                "INV_X1 u4 ( .a(n4), .o(out) );\n"
                                "INV_Z80 lcb1 ( .a(iccad_clk), .o(lcb1_fo) );\n"
                                "endmodule\n";
}

#endif
