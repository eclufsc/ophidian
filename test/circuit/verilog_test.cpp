#include "verilog_test.h"
#include <catch.hpp>

#include <ophidian/circuit/VerilogParser.h>

using namespace ophidian::circuit;

TEST_CASE("VerilogParser: Parse Input", "[circuit][VerilogParser]")
{
    VerilogParser vp;
    std::stringstream ss;
    ss << "\
        module ckt (in);\
        input in1;\
        input in2, in3, in4;\
        endmodule";
    std::unique_ptr<Netlist> nl(vp.createNetlist(ss));
    REQUIRE(nl.get());
    REQUIRE( nl->size(Net()) == 4 );
    REQUIRE( nl->size(Pin()) == 4 );
    REQUIRE( nl->size(Input()) == 4 );
}

TEST_CASE("VerilogParser: Parse Output", "[circuit][VerilogParser]")
{
    VerilogParser vp;
    std::stringstream ss;
    ss << "\
        module ckt (out);\
        output out;\
        endmodule";
    std::unique_ptr<Netlist> nl(vp.createNetlist(ss));
    REQUIRE(nl.get());
    REQUIRE( nl->size(Net()) == 1 );
    REQUIRE( nl->size(Pin()) == 1 );
    REQUIRE( nl->size(Output()) == 1 );
}

TEST_CASE("VerilogParser: Parse Input & Output ", "[circuit][VerilogParser]")
{
    VerilogParser vp;
    std::stringstream ss;
    ss << "\
        module ckt (in, out);\
        input in;\
        output out;\
        endmodule";
    std::unique_ptr<Netlist> nl(vp.createNetlist(ss));
    REQUIRE(nl.get());
    REQUIRE( nl->size(Net()) == 2 );
    REQUIRE( nl->size(Pin()) == 2 );
    REQUIRE( nl->size(Output()) == 1 );
    REQUIRE( nl->size(Input()) == 1 );
}


TEST_CASE("VerilogParser: Parse Wire ", "[circuit][VerilogParser]")
{
    VerilogParser vp;
    std::stringstream ss;
    ss << "\
        module ckt ();\
        wire net;\
        endmodule";
    std::unique_ptr<Netlist> nl(vp.createNetlist(ss));
    REQUIRE(nl.get());
    REQUIRE( nl->size(Net()) == 1 );
}

TEST_CASE("VerilogParser: Parse Inverter", "[circuit][VerilogParser]")
{
    VerilogParser vp;
    std::stringstream ss;
    ss << "\
        module ckt (in, out);\
        input in;\
        output out;\
        wire in;\
        wire out;\
        inv u1 ( .a(in), .o(out) );\
        endmodule";
    std::unique_ptr<Netlist> nl(vp.createNetlist(ss));
    REQUIRE(nl.get());
    REQUIRE( nl->size(Net()) == 2 );
    REQUIRE( nl->size(Pin()) == 4 );
    REQUIRE( nl->size(Input()) == 1 );
    REQUIRE( nl->size(Output()) == 1 );
    REQUIRE( nl->size(Cell()) == 1);
}

TEST_CASE("VerilogParser: Parse Simple", "[circuit][VerilogParser]")
{
    VerilogParser vp;
    std::stringstream ss;
    ss << "\
        module simple (inp1, inp2, iccad_clk, out);\
        input inp1;\
        input inp2;\
        input iccad_clk;\
        output out;\
        wire inp1;\
        wire inp2;\
        wire out;\
        wire n1, n2, n3, n4;\
        wire iccad_clk;\
        nand2 u1 ( .a(inp1), .b(inp2), .o(n1) );\
        nor2 u2 ( .a(n1), .b(n3), .o(n2) );\
        dff f1 ( .ck(iccad_clk), .d(n2), .q(n3) );\
        inv u3 ( .a(n3), .o(n4) );\
        inv u4 ( .a(n4), .o(out) );\
        endmodule";
    std::unique_ptr<Netlist> nl(vp.createNetlist(ss));
    REQUIRE(nl.get());
    REQUIRE( nl->size(Net()) == 8 );
    REQUIRE( nl->size(Pin()) == 17 );
    REQUIRE( nl->size(Input()) == 3 );
    REQUIRE( nl->size(Output()) == 1 );
    REQUIRE( nl->size(Cell()) == 5);

}

#include <fstream>
TEST_CASE("VerilogParser: Parse Superblue 18", "[circuit][VerilogParser]")
{
    VerilogParser vp;
    std::ifstream ss("superblue18.v", std::ifstream::in);
    std::unique_ptr<Netlist> nl(vp.createNetlist(ss));
    REQUIRE(nl.get());
    REQUIRE( nl->size(Net()) == 771542 );
    REQUIRE( nl->size(Cell()) == 767499 );
    REQUIRE(true);
}




