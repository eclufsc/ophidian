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

#include <netlist.h>
#include "../placement/cells.h"
#include <standard_cells.h>
#include <boost/geometry.hpp>
#include <library.h>
#include <lef.h>
#include "../geometry/geometry.h"
#include  "../placement/placement.h"

TEST_CASE("placement cell info", "[placement]") {

	// create std cells
	ophidian::standard_cell::standard_cells std_cells;
	auto INV_X1 = std_cells.cell_create("INV_X1");

	// create dummy lib
	ophidian::placement::library lib(&std_cells);
	std::vector<ophidian::geometry::point<double> > points { { 0.0, 0.0 }, { 0.0,
			200.0 }, { 800.0, 200.0 }, { 800.0, 0.0 }, { 0.0, 0.0 } };

	ophidian::geometry::polygon<ophidian::geometry::point<double> > polygon;
	boost::geometry::append(polygon, points);
	ophidian::geometry::multi_polygon<
	ophidian::geometry::polygon<ophidian::geometry::point<double> > > multipolygon {
		polygon };
	lib.geometry(INV_X1, multipolygon);

	// create netlist
	ophidian::netlist::netlist netlist(&std_cells);
	auto u1 = netlist.cell_insert("u1", "INV_X1");

	ophidian::placement::cells cells(&netlist);
	cells.position(u1, { 0.0, 0.0 });
	cells.geometry(u1, lib.geometry(INV_X1));
	cells.fixed(u1, false);
	REQUIRE(
			boost::geometry::equals(cells.position(u1),
					ophidian::geometry::point<double>(0.0, 0.0)));
	REQUIRE(boost::geometry::equals(cells.geometry(u1), lib.geometry(INV_X1)));
	REQUIRE(cells.fixed(u1) == false);
}
