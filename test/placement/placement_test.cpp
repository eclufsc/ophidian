/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#include "../catch.hpp"

#include "../placement/placement.h"

#include <boost/geometry/strategies/distance.hpp>
#include <boost/geometry/algorithms/distance.hpp>
#include  <iostream>

TEST_CASE("placement/place pads", "[placement]") {

	ophidian::standard_cell::standard_cells std_cells;
	ophidian::netlist::netlist netlist(&std_cells);
	auto inp1 = netlist.PI_insert("inp1");
	auto out = netlist.PO_insert("out");

	ophidian::placement::library lib { &std_cells };
	ophidian::placement::placement placement { &netlist, &lib };
	ophidian::geometry::point<double> position { 1.0, 2.0 };
	ophidian::geometry::point<double> position2 { 3.0, 4.0 };
	placement.pad_position(inp1, position);
	placement.pad_position(out, position2);

	REQUIRE(placement.pin_position(inp1).x() == position.x());
	REQUIRE(placement.pin_position(inp1).y() == position.y());
	REQUIRE(placement.pin_position(out).x() == position2.x());
	REQUIRE(placement.pin_position(out).y() == position2.y());

}

TEST_CASE("placement/place cell", "[placement]") {
	ophidian::standard_cell::standard_cells std_cells;
	ophidian::netlist::netlist netlist(&std_cells);
	ophidian::placement::library lib { &std_cells };
	ophidian::placement::placement placement { &netlist, &lib };

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
