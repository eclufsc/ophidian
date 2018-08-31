#include <catch.hpp>

#include <ophidian/circuit/NetlistFactory.h>

using namespace ophidian::circuit;

TEST_CASE("Netlist factory: populate with simple verilog.", "[circuit][Netlist][factory]")
{
    auto netlist = Netlist{};

    auto simple = ophidian::parser::Verilog{"input_files/simple/simple.v"};

    factory::make_netlist(netlist, simple);
}
