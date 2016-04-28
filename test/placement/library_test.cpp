/*
 * Copyright 2016 Ophidian
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
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

