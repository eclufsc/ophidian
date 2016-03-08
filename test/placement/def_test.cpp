#include "../catch.hpp"

#include <def.h>
#include "../placement/placement.h"
#include <boost/geometry/algorithms/equals.hpp>
#include <iostream>
#include <fstream>

TEST_CASE("def read", "[def]") {

    std::ifstream superblue16_def("benchmarks/superblue16/superblue16.def", std::ifstream::in);
	REQUIRE( superblue16_def.good() );
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	openeda::placement::library lib(&std_cells);
	openeda::placement::placement cells(&netlist, &lib);


	openeda::placement::def::read(superblue16_def, &netlist, &cells);

	auto position = cells.cell_position(netlist.cell_insert("o641083", "INV_Z4"));
	auto golden = openeda::geometry::point<double> { 7214680.0, 1402200.0 };

	auto pad_position = cells.pin_position(netlist.pin_by_name("iccad_clk"));
	auto golden_pad_position = openeda::geometry::point<double> {5807950, 6121800};
	golden_pad_position.x(golden_pad_position.x() + 0.5*(-140+140));
	golden_pad_position.y(golden_pad_position.y() + 0.5*(0+280));


	REQUIRE( boost::geometry::equals( position, golden ) );

	REQUIRE( boost::geometry::equals( pad_position, golden_pad_position ) );

}
