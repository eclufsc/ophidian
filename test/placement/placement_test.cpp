#include "../catch.hpp"

#include <placement.h>

TEST_CASE("placement/cell place", "[placement]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	openeda::placement::library lib(&std_cells);
	netlist.cell_insert("u1", "INV_X1");
	openeda::placement::placement placement(&netlist, &lib);
}
