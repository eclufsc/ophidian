#include "../catch.hpp"

#include <netlist.h>
#include <verilog.h>
#include <iostream>

TEST_CASE("parser/tokenize", "[parser]") {

	const std::string str("this is a sample string; .ab(a),b,c) ");
	const std::vector<std::string> golden { "this", "is", "a", "sample",
			"string", ".ab", "a", "b", "c" };
	const std::vector<std::string> tokens =
			openeda::netlist::verilog::parser::tokenize(str);
	REQUIRE(tokens == golden);
}

TEST_CASE("verilog/read","[verilog]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist simple(&std_cells);
    std::ifstream file("benchmarks/simple/simple.v", std::ifstream::in);
	REQUIRE(file.good());
	openeda::netlist::verilog::read(file, &simple);

	REQUIRE(simple.PI_count() == 3);
	REQUIRE(simple.PO_count() == 1);
	REQUIRE(simple.net_count() == 9);
	REQUIRE(simple.cell_count() == 6);
	REQUIRE(simple.pin_count() == 19);
	REQUIRE(simple.module_name() == "simple");
	REQUIRE(std_cells.cell_count() == 5);

	bool ok = true;
	for (auto cell : simple.cell_system()) {
		std::string name = simple.cell_name(cell.second);
		std::string std_cell = std_cells.cell_name(simple.cell_std_cell(cell.second));
		auto cell_pins = simple.cell_pins(cell.second);
		if (name == "u1") {
			REQUIRE(std_cell == "NAND2_X1");
			REQUIRE(cell_pins.size() == 3);
		} else if (name == "u2") {
			REQUIRE(std_cell == "NOR2_X1");
			REQUIRE(cell_pins.size() == 3);
		} else if (name == "f1") {
			REQUIRE(std_cell == "DFF_X80");
			REQUIRE(cell_pins.size() == 3);
		} else if (name == "u3" || name == "u4") {
			REQUIRE(std_cell == "INV_X1");
			REQUIRE(cell_pins.size() == 2);
		} else if (name == "lcb1") {
			REQUIRE(std_cell == "INV_Z80");
			REQUIRE(cell_pins.size() == 2);
		} else {
			REQUIRE(false);
		}
	}

	std::cout << simple.module_name() << " pins (" << simple.pin_count() << ")"
			<< std::endl;

	for(auto p : simple.pin_system())
	{
		std::cout << simple.pin_name(p.second) << std::endl;
	}

	std::cout << "std cell types (" << std_cells.cell_count() << ")" << std::endl;
	auto std_cell_names = std_cells.cell_names();
	std::for_each(std_cell_names.begin(), std_cell_names.end(),
			[](std::string name) {
				std::cout << name << std::endl;
			});

	std::cout << "std cell pins (" << std_cells.pin_system().size() << ")" << std::endl;
	for(auto std_cell_pin : std_cells.pin_system())
	{
		std::cout << std_cells.pin_name(std_cell_pin.second) << std::endl;
	}

}

//TEST_CASE("verilog/read suplerblue16","[verilog][slow]") {
//	openeda::standard_cell::standard_cells std_cells;
//	openeda::netlist::netlist superblue16(&std_cells);
//	std::ifstream file("test/superblue16.v", std::ifstream::in);
//	REQUIRE(file.good());
//	std::stringstream buffer;
//	buffer << file.rdbuf();
//	file.close();
//	time_t begin, end;
//	time(&begin);
//	openeda::netlist::verilog::read(buffer, &superblue16);
//	time(&end);
//	std::cout << "time to read \"test/superblue16.v\" = "
//			<< difftime(end, begin) << " seconds" << std::endl;
////	REQUIRE(superblue16.cell_count() == 981559);
//	REQUIRE(superblue16.net_count() == 999902);
//
//}
