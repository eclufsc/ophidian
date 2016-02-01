#include "../catch.hpp"

#include <library.h>
#include <iostream>
#include <fstream>
#include <lef.h>
#include <boost/geometry/io/wkt/write.hpp>
#include <boost/geometry/algorithms/equals.hpp>
#include "../geometry/geometry.h"

using point = openeda::geometry::point<double>;
using polygon = openeda::geometry::polygon<point>;
using multi_polygon = openeda::geometry::multi_polygon<polygon>;

TEST_CASE("placement library/set & get geometry", "[placement]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::placement::library lib(&std_cells);
	auto INV_X1 = std_cells.create("INV_X1");
	std::vector<point> points { { 0.0, 0.0 }, { 0.0, 200.0 }, { 800.0,
			200.0 }, { 800.0, 0.0 }, { 0.0, 0.0 } };
	polygon polygon;
	boost::geometry::append(polygon, points);
	multi_polygon multi;
	lib.geometry(INV_X1, multi);
}

TEST_CASE("placement/read lef", "[placement]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::placement::library lib(&std_cells);

	std::ifstream simple_lef("test/superblue16.lef", std::ifstream::in);
	REQUIRE(simple_lef.good());
	std::stringstream buffer;
	buffer << simple_lef.rdbuf();
	simple_lef.close();
	std::cout << "reading lef..." << std::endl;
	openeda::placement::lef::read(buffer, &std_cells, &lib);

	REQUIRE(lib.dist2microns() == 2000);

	std::cout << "lef library" << std::endl;
	for (auto c : std_cells.system()) {
		std::cout << std_cells.name(c.second) << " "
				<< boost::geometry::wkt(lib.geometry(c.second)) << std::endl;
	}

}
