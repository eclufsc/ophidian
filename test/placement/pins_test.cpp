#include "../catch.hpp"

#include "../netlist/netlist.h"
#include "../placement/pins.h"
#include "../geometry/geometry.h"

TEST_CASE("placement/pins/initial position","[placement][pins]"){

	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist{&std_cells};
	openeda::placement::pins pins{&netlist};

	auto inp1 = netlist.PI_insert("inp1");

	openeda::geometry::point<double> inp1_position = pins.position(inp1);

	REQUIRE( inp1_position.x() == 0.0 );
	REQUIRE( inp1_position.y() == 0.0 );

}

TEST_CASE("placement/pins/set position","[placement][pins]"){

	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist{&std_cells};
	openeda::placement::pins pins{&netlist};

	auto inp1 = netlist.PI_insert("inp1");
	pins.position(inp1, {1.0, 2.0});
	openeda::geometry::point<double> inp1_position = pins.position(inp1);
	REQUIRE( inp1_position.x() == 1.0 );
	REQUIRE( inp1_position.y() == 2.0 );
}
