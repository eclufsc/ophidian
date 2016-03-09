#include "../catch.hpp"

#include "../placement/lef.h"
#include "../standard_cell/standard_cells.h"
#include "../netlist/netlist.h"
#include "../placement/placement.h"

#include <fstream>

TEST_CASE("lef read", "[lef]") {

    std::ifstream superblue16_lef("benchmarks/superblue16/superblue16.lef", std::ifstream::in);
	REQUIRE( superblue16_lef.good() );
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	openeda::placement::library lib(&std_cells);
	openeda::placement::placement cells(&netlist, &lib);
	openeda::placement::lef::read(superblue16_lef, &std_cells, &lib);

	auto NOR3_X4 = std_cells.cell_create("NOR3_X4");

	auto pins = std_cells.cell_pins(NOR3_X4);

	REQUIRE( !pins.empty() );

	for(auto p : pins) {
		std::cout << "pin " << std_cells.pin_name(p) << std::endl;
		std::cout << "   offset " << lib.pin_offset(p).x() << ", " << lib.pin_offset(p).y() << std::endl;
	}


}
