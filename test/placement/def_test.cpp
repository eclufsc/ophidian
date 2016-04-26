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

#include <def.h>
#include "../placement/placement.h"
#include <boost/geometry/algorithms/equals.hpp>
#include <iostream>
#include <fstream>

TEST_CASE("def read", "[def]") {

    std::ifstream superblue16_def("benchmarks/superblue16/superblue16.def", std::ifstream::in);
	REQUIRE( superblue16_def.good() );
	ophidian::standard_cell::standard_cells std_cells;
	ophidian::netlist::netlist netlist(&std_cells);
	ophidian::placement::library lib(&std_cells);
	ophidian::placement::placement cells(&netlist, &lib);
	ophidian::floorplan::floorplan floorplan;
	ophidian::placement::def::read(superblue16_def, &netlist, &cells, &floorplan);

	auto position = cells.cell_position(netlist.cell_insert("o641083", "INV_Z4"));
	auto golden = ophidian::geometry::point<double> { 7214680.0, 1402200.0 };

	auto pad_position = cells.pin_position(netlist.pin_by_name("iccad_clk"));
	auto golden_pad_position = ophidian::geometry::point<double> {5807950, 6121800};
	golden_pad_position.x(golden_pad_position.x() + 0.5*(-140+140));
	golden_pad_position.y(golden_pad_position.y() + 0.5*(0+280));

	REQUIRE( boost::geometry::equals( position, golden ) );
	REQUIRE( boost::geometry::equals( pad_position, golden_pad_position ) );

}
