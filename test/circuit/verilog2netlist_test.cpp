#include <catch.hpp>

#include <ophidian/circuit/Verilog2Netlist.h>
#include <ophidian/circuit/Netlist.h>
#include <sstream>

using namespace ophidian;

class Verilog2NetlistFixture
{
public:
	Verilog2NetlistFixture(){
		std::stringstream input(simpleInput);
		verilog = std::make_unique<parser::Verilog>(input);
	}

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
	std::unique_ptr<parser::Verilog> verilog;
	circuit::Netlist netlist;
};

TEST_CASE_METHOD(Verilog2NetlistFixture, "Verilog2Netlist: The Verilog object and Netlist module must have same amount of elements.", "[circuit][Netlist][Verilog]")
{
	circuit::verilog2Netlist(*verilog, netlist);

	const parser::Verilog::Module & simple = verilog->modules().front();
	REQUIRE(simple.nets().size() == netlist.size(circuit::Net()));
	REQUIRE(simple.ports().size() == (netlist.size(circuit::Input()) + netlist.size(circuit::Output())));
	REQUIRE(simple.module_instances().size() == netlist.size(circuit::Cell()));

	std::size_t moduleSizePins = 0;
	for(auto instance : simple.module_instances())
		moduleSizePins += instance.net_map().size();
	moduleSizePins += simple.ports().size();
	REQUIRE(moduleSizePins == netlist.size(circuit::Pin()));
}

TEST_CASE_METHOD(Verilog2NetlistFixture, "Verilog2Netlist: Fetching some entities by their names and check their names.", "[circuit][Netlist][Verilog]")
{
	circuit::verilog2Netlist(*verilog, netlist);

	const parser::Verilog::Module & simple = verilog->modules().front();
	REQUIRE(netlist.name(netlist.find(circuit::Pin(), "inp1")) == "inp1");
	REQUIRE(netlist.name(netlist.find(circuit::Pin(), "u3:a")) == "u3:a");
	REQUIRE(netlist.name(netlist.find(circuit::Cell(), "u1")) == "u1");
	REQUIRE(netlist.name(netlist.find(circuit::Net(), "iccad_clk")) == "iccad_clk");
}
