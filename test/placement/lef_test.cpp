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

#include "../placement/lef.h"
#include "../standard_cell/standard_cells.h"
#include "../netlist/netlist.h"
#include "../placement/placement.h"

#include <fstream>

TEST_CASE("lef read", "[lef]") {

    std::ifstream superblue16_lef("benchmarks/superblue16/superblue16.lef", std::ifstream::in);
	REQUIRE( superblue16_lef.good() );
	ophidian::standard_cell::standard_cells std_cells;
	ophidian::netlist::netlist netlist(&std_cells);
	ophidian::placement::library lib(&std_cells);
	ophidian::placement::placement cells(&netlist, &lib);
	ophidian::placement::lef::read(superblue16_lef, &std_cells, &lib);

	auto NOR3_X4 = std_cells.cell_create("NOR3_X4");

	auto pins = std_cells.cell_pins(NOR3_X4);

	REQUIRE( !pins.empty() );

	for(auto p : pins) {
		std::cout << "pin " << std_cells.pin_name(p) << std::endl;
		std::cout << "   offset " << lib.pin_offset(p).x() << ", " << lib.pin_offset(p).y() << std::endl;
	}


}
