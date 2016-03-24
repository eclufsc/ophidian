#include "../catch.hpp"

#include "../timing-driven_placement/flute_rc_tree_estimation.h"
#include <boost/units/systems/si/prefixes.hpp>

TEST_CASE("flute rc_tree/tap mapping", "[tdp][flute][rc_tree]")
{
	openeda::standard_cell::standard_cells std_cells;

	openeda::netlist::netlist netlist{&std_cells};
	auto n1 = netlist.net_insert("n1");
	auto u1 = netlist.cell_insert("u1", "INV_X1");
	auto u2 = netlist.cell_insert("u2", "INV_X1");
	auto u3 = netlist.cell_insert("u3", "INV_X1");
	auto u4 = netlist.cell_insert("u4", "INV_X1");
	auto u1o = netlist.pin_insert(u1, "o");
	auto u2a = netlist.pin_insert(u2, "a");
	auto u3a = netlist.pin_insert(u3, "a");
	auto u4a = netlist.pin_insert(u4, "a");

	openeda::timing::library_timing_arcs tarcs{&std_cells};
	openeda::timing::library timing_library{&tarcs, &std_cells};



	netlist.connect(n1, u1o);
	netlist.connect(n1, u2a);
	netlist.connect(n1, u3a);
	netlist.connect(n1, u4a);

	timing_library.pin_capacitance( netlist.pin_std_cell(u2a), boost::units::quantity<boost::units::si::capacitance>(1*boost::units::si::femto*boost::units::si::farad) );

	openeda::entity::entity net{n1};
	openeda::placement::library library{&std_cells};
	openeda::placement::placement placement{&netlist, &library};

	placement.cell_position(u1, openeda::geometry::point<double>(20, 20));
	placement.cell_position(u2, openeda::geometry::point<double>(30, 40));
	placement.cell_position(u3, openeda::geometry::point<double>(50, 10));
	placement.cell_position(u4, openeda::geometry::point<double>(50, 50));

	openeda::interconnection::rc_tree rc_tree;
	auto tap_mapping = openeda::timingdriven_placement::flute_rc_tree(placement, net, rc_tree, timing_library);

	REQUIRE( rc_tree.capacitor_count() == 10 );
	REQUIRE( rc_tree.capacitance(tap_mapping.at(u2a)) == timing_library.pin_capacitance( netlist.pin_std_cell(u2a) ) );
	REQUIRE( rc_tree.capacitance(tap_mapping.at(u1o)) == timing_library.pin_capacitance( netlist.pin_std_cell(u1o) ) );

}


