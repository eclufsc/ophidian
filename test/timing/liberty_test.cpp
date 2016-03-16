#include "../catch.hpp"

#include "../timing/liberty.h"

#include <boost/units/systems/si/prefixes.hpp>

TEST_CASE("liberty/pin capacitance", "[timing][liberty]")
{
	openeda::standard_cell::standard_cells std_cells;
	openeda::timing::library lib{&std_cells};
	openeda::timing::liberty::read("benchmarks/superblue16/superblue16_Early.lib", lib);
	REQUIRE( lib.pin_capacitance(std_cells.pin_create(std_cells.cell_create("NOR2_X4"), "b")) == boost::units::quantity<boost::units::si::capacitance>(3*boost::units::si::femto*boost::units::si::farads) );
	REQUIRE( lib.pin_capacitance(std_cells.pin_create(std_cells.cell_create("NOR2_X4"), "o")) == boost::units::quantity<boost::units::si::capacitance>(0.0*boost::units::si::femto*boost::units::si::farads) );
}

