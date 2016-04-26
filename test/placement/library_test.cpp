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

#include <library.h>
#include <iostream>
#include <fstream>
#include <lef.h>
#include <boost/geometry/io/wkt/write.hpp>
#include <boost/geometry/algorithms/equals.hpp>
#include "../geometry/geometry.h"

using point = ophidian::geometry::point<double>;
using polygon = ophidian::geometry::polygon<point>;
using multi_polygon = ophidian::geometry::multi_polygon<polygon>;

TEST_CASE("placement library/set & get geometry", "[placement]") {
	ophidian::standard_cell::standard_cells std_cells;
	ophidian::placement::library lib(&std_cells);
	auto INV_X1 = std_cells.cell_create("INV_X1");
	std::vector<point> points { { 0.0, 0.0 }, { 0.0, 200.0 }, { 800.0, 200.0 },
			{ 800.0, 0.0 }, { 0.0, 0.0 } };
	polygon polygon;
	boost::geometry::append(polygon, points);
	multi_polygon multi;
	lib.geometry(INV_X1, multi);
}

TEST_CASE("placement/read lef", "[placement]") {
	ophidian::standard_cell::standard_cells std_cells;
	ophidian::placement::library lib(&std_cells);
	ophidian::floorplan::floorplan floorplan;

	std::ifstream simple_lef("benchmarks/superblue16/superblue16.lef",
			std::ifstream::in);
	REQUIRE(simple_lef.good());
	std::stringstream buffer;
	buffer << simple_lef.rdbuf();
	simple_lef.close();
	std::cout << "reading lef..." << std::endl;
	ophidian::placement::lef::read(buffer, &std_cells, &lib, &floorplan);

	REQUIRE(lib.dist2microns() == 2000);

//	std::cout << "lef library" << std::endl;
//	for (auto c : std_cells.cell_system()) {
//		std::cout << std_cells.cell_name(c.second) << " "
//				<< boost::geometry::wkt(lib.geometry(c.second)) << std::endl;
//	}
}

