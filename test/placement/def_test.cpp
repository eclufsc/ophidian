#include "../catch.hpp"

#include <def.h>
#include "../placement/cells.h"
#include <boost/geometry/algorithms/equals.hpp>
#include <iostream>
#include <fstream>

TEST_CASE("def read", "[placement]") {

	std::ifstream superblue16_def("test/superblue16.def", std::ifstream::in);
	REQUIRE( superblue16_def.good() );
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	openeda::placement::library lib(&std_cells);
	openeda::placement::cells cells(&netlist);


	openeda::placement::def::read(superblue16_def, &netlist, &lib, &cells);

	auto position = cells.position(netlist.cell_insert("o641083", "INV_Z4"));
	auto golden = openeda::geometry::point<double> { 7214680.0, 1402200.0 };

	std::cout << position.x() << ", " << position.y() << std::endl;

	REQUIRE( boost::geometry::equals( position, golden ) );

}
