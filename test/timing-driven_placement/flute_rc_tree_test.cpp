#include "../catch.hpp"

#include "../timing-driven_placement/flute_rc_tree_estimation.h"

TEST_CASE("flute rc_tree/", "[tdp][flute][rc_tree]")
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
	netlist.connect(n1, u1o);
	netlist.connect(n1, u2a);
	netlist.connect(n1, u3a);
	netlist.connect(n1, u4a);

	openeda::entity::entity net{n1};
	openeda::placement::library library{&std_cells};
	openeda::placement::placement placement{&netlist, &library};

	placement.cell_position(u1, openeda::geometry::point<double>(20, 20));
	placement.cell_position(u2, openeda::geometry::point<double>(30, 40));
	placement.cell_position(u3, openeda::geometry::point<double>(50, 10));
	placement.cell_position(u4, openeda::geometry::point<double>(50, 50));

	openeda::interconnection::rc_tree rc_tree;
	openeda::timingdriven_placement::flute_rc_tree(placement, net, rc_tree);

	REQUIRE( rc_tree.capacitor_count() == 10 );

}


