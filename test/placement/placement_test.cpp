#include "../catch.hpp"

#include <netlist.h>
#include "../placement/cells.h"
#include <standard_cells.h>
#include <boost/geometry.hpp>
#include <library.h>
#include <lef.h>
#include "../geometry/geometry.h"

TEST_CASE("placement library/set & get geometry", "[placement]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::placement::library lib(&std_cells);
	auto INV_X1 = std_cells.create("INV_X1");
	std::vector<openeda::geometry::point<double> > points { { 0.0, 0.0 }, { 0.0,
			200.0 }, { 800.0, 200.0 }, { 800.0, 0.0 }, { 0.0, 0.0 } };

	openeda::geometry::polygon<openeda::geometry::point<double> > polygon;
	boost::geometry::append(polygon, points);
	openeda::geometry::multi_polygon<
			openeda::geometry::polygon<openeda::geometry::point<double> > > multipolygon {
			polygon };
	lib.geometry(INV_X1, multipolygon);
	REQUIRE(boost::geometry::equals(lib.geometry(INV_X1), multipolygon));
}

TEST_CASE("placement cell info", "[placement]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	openeda::placement::library lib(&std_cells);

	auto INV_X1 = std_cells.create("INV_X1");
	auto u1 = netlist.cell_insert("u1", "INV_X1");
	std::vector<openeda::geometry::point<double> > points { { 0.0, 0.0 }, { 0.0,
			200.0 }, { 800.0, 200.0 }, { 800.0, 0.0 }, { 0.0, 0.0 } };

	openeda::geometry::polygon<openeda::geometry::point<double> > polygon;
	boost::geometry::append(polygon, points);
	openeda::geometry::multi_polygon<
			openeda::geometry::polygon<openeda::geometry::point<double> > > multipolygon {
			polygon };
	lib.geometry(INV_X1, multipolygon);
	openeda::placement::cells cells(&netlist);
	cells.position(u1, { 0.0, 0.0 });
	cells.geometry(u1, lib.geometry(INV_X1));
	REQUIRE(
			boost::geometry::equals(cells.position(u1),
					openeda::geometry::point<double>(0.0, 0.0)));
	REQUIRE(boost::geometry::equals(cells.geometry(u1), lib.geometry(INV_X1)));
}

#include <fstream>
#include <boost/geometry/io/wkt/write.hpp>
TEST_CASE("placement/ lef library", "[placement]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	openeda::placement::library lib(&std_cells);

    std::ifstream simple_lef("benchmarks/superblue16/superblue16.lef", std::ifstream::in);
	REQUIRE(simple_lef.good());
	std::stringstream buffer;
	buffer << simple_lef.rdbuf();
	simple_lef.close();
	std::cout << "reading lef..." << std::endl;
	openeda::placement::lef::read(buffer, &std_cells, &lib);

	std::cout << "lef library" << std::endl;
	for (auto c : std_cells.system().entities()) {
		std::cout << std_cells.name(c.first) << " "
				<< boost::geometry::wkt(lib.geometry(c.first)) << std::endl;
	}

}

