#include "../catch.hpp"

#include "../placement/placement.h"

#include <boost/geometry/strategies/distance.hpp>
#include <boost/geometry/algorithms/distance.hpp>
#include  <iostream>

TEST_CASE("placement/place pads", "[placement]") {

	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	auto inp1 = netlist.PI_insert("inp1");
	auto out = netlist.PO_insert("out");

	openeda::placement::library lib { &std_cells };
	openeda::placement::placement placement { &netlist, &lib };
	openeda::geometry::point<double> position { 1.0, 2.0 };
	openeda::geometry::point<double> position2 { 3.0, 4.0 };
	placement.pad_position(inp1, position);
	placement.pad_position(out, position2);

	REQUIRE(placement.pin_position(inp1).x() == position.x());
	REQUIRE(placement.pin_position(inp1).y() == position.y());
	REQUIRE(placement.pin_position(out).x() == position2.x());
	REQUIRE(placement.pin_position(out).y() == position2.y());

}

TEST_CASE("placement/place cell", "[placement]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	openeda::placement::library lib { &std_cells };
	openeda::placement::placement placement { &netlist, &lib };

	auto INV_X1 = std_cells.cell_create("INV_X1");
	auto INV_X1a = std_cells.pin_create(INV_X1, "a");
	auto INV_X1o = std_cells.pin_create(INV_X1, "o");

	lib.pin_offset(INV_X1a, { 1.0, 2.0 });
	lib.pin_offset(INV_X1o, { 3.0, 4.0 });

	auto u1 = netlist.cell_insert("u1", "INV_X1");
	auto u1a = netlist.pin_insert(u1, "a");
	auto u1o = netlist.pin_insert(u1, "o");

	placement.cell_position(u1, { 100.0, 200.0 });

	REQUIRE(placement.pin_position(u1a).x() == 101.0);
	REQUIRE(placement.pin_position(u1a).y() == 202.0);

	REQUIRE(placement.pin_position(u1o).x() == 103.0);
	REQUIRE(placement.pin_position(u1o).y() == 204.0);

}
