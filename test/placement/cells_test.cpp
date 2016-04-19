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
	openeda::standard_cell::standard_cells std_cells;
	auto INV_X1 = std_cells.cell_create("INV_X1");

	// create dummy lib
	openeda::placement::library lib(&std_cells);
	std::vector<openeda::geometry::point<double> > points { { 0.0, 0.0 }, { 0.0,
			200.0 }, { 800.0, 200.0 }, { 800.0, 0.0 }, { 0.0, 0.0 } };

	openeda::geometry::polygon<openeda::geometry::point<double> > polygon;
	boost::geometry::append(polygon, points);
	openeda::geometry::multi_polygon<
	openeda::geometry::polygon<openeda::geometry::point<double> > > multipolygon {
		polygon };
	lib.geometry(INV_X1, multipolygon);

	// create netlist
	openeda::netlist::netlist netlist(&std_cells);
	auto u1 = netlist.cell_insert("u1", "INV_X1");

	openeda::placement::cells cells(&netlist);
	cells.position(u1, { 0.0, 0.0 });
	cells.geometry(u1, lib.geometry(INV_X1));
	cells.fixed(u1, false);
	REQUIRE(
			boost::geometry::equals(cells.position(u1),
					openeda::geometry::point<double>(0.0, 0.0)));
	REQUIRE(boost::geometry::equals(cells.geometry(u1), lib.geometry(INV_X1)));
	REQUIRE(cells.fixed(u1) == false);
}
