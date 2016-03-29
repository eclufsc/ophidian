#include "../catch.hpp"

#include "../timing-driven_placement/flute_rc_tree_estimation.h"
#include <boost/units/systems/si/prefixes.hpp>

#include <lemon/connectivity.h>
#include <boost/algorithm/string.hpp>

using namespace openeda;
using namespace boost::units;
using namespace boost::units::si;


bool compare(quantity<capacitance> a, quantity<capacitance> b)
{
    if(((a-b)/b) >= 0.001)
        std::cout << a << " and " << b << " are different ( " << ((a-b)/b)*100 << "% )" << std::endl;
    return ((a-b)/b) < 0.001;
}

TEST_CASE("flute rc_tree/tap mapping for two pins", "[tdp][flute][rc_tree]")
{
    standard_cell::standard_cells std_cells;

    netlist::netlist netlist{&std_cells};
    auto n1 = netlist.net_insert("n1");
    auto u1 = netlist.cell_insert("u1", "INV_X1");
    auto u2 = netlist.cell_insert("u2", "INV_X1");
    auto u1o = netlist.pin_insert(u1, "o");
    auto u2a = netlist.pin_insert(u2, "a");

    timing::library_timing_arcs tarcs{&std_cells};
    timing::library timing_library{&tarcs, &std_cells};

    netlist.connect(n1, u1o);
    netlist.connect(n1, u2a);

    timing_library.pin_capacitance( netlist.pin_std_cell(u2a), quantity<capacitance>(0*femto*farad) );

    entity::entity net{n1};
    placement::library library{&std_cells};
    library.dist2microns(1000);
    placement::placement placement{&netlist, &library};

    placement.cell_position(u1, geometry::point<double>(2000, 2000));
    placement.cell_position(u2, geometry::point<double>(3000, 4000));

    interconnection::rc_tree rc_tree;
    timingdriven_placement::flute_rc_tree_creator flute;
    auto tap_mapping = flute.create_tree(placement, net, rc_tree, timing_library);


    REQUIRE( lemon::connected(rc_tree.graph()) );

    REQUIRE( rc_tree.capacitor_count() == 4 );
    REQUIRE( rc_tree.capacitance(tap_mapping.at(u2a)) == timing_library.pin_capacitance( netlist.pin_std_cell(u2a) ) );
    REQUIRE( rc_tree.capacitance(tap_mapping.at(u1o)) == timing_library.pin_capacitance( netlist.pin_std_cell(u1o) ) );
    REQUIRE( compare(rc_tree.lumped(), quantity<capacitance>(0.6*femto*farads)) );

    REQUIRE( lemon::countOutArcs(rc_tree.graph(), tap_mapping.at(u1o)) == 1 );
    auto u1o_cap = rc_tree.other_capacitor(rc_tree.capacitor_resistors(tap_mapping.at(u1o)), tap_mapping.at(u1o));
    REQUIRE( compare(rc_tree.capacitance(u1o_cap), quantity<capacitance>(0.3*femto*farad)) );

    REQUIRE( lemon::countOutArcs(rc_tree.graph(), tap_mapping.at(u2a)) == 1 );
    auto u2a_cap = rc_tree.other_capacitor(rc_tree.capacitor_resistors(tap_mapping.at(u2a)), tap_mapping.at(u2a));
    REQUIRE( compare(rc_tree.capacitance(u2a_cap), quantity<capacitance>(0.3*femto*farad)) );

}

TEST_CASE("flute rc_tree/tap mapping for three pins", "[tdp][flute][rc_tree]")
{
    standard_cell::standard_cells std_cells;

    netlist::netlist netlist{&std_cells};
    auto n1 = netlist.net_insert("n1");
    auto u1 = netlist.cell_insert("u1", "INV_X1");
    auto u2 = netlist.cell_insert("u2", "INV_X1");
    auto u3 = netlist.cell_insert("u3", "INV_X1");
    auto u1o = netlist.pin_insert(u1, "o");
    auto u2a = netlist.pin_insert(u2, "a");
    auto u3a = netlist.pin_insert(u3, "a");

    timing::library_timing_arcs tarcs{&std_cells};
    timing::library timing_library{&tarcs, &std_cells};

    netlist.connect(n1, u1o);
    netlist.connect(n1, u2a);
    netlist.connect(n1, u3a);

    timing_library.pin_capacitance( netlist.pin_std_cell(u2a), quantity<capacitance>(0*femto*farad) );

    entity::entity net{n1};
    placement::library library{&std_cells};
    library.dist2microns(1000);
    placement::placement placement{&netlist, &library};

    placement.cell_position(u1, geometry::point<double>(2000, 2000));
    placement.cell_position(u2, geometry::point<double>(3000, 4000));
    placement.cell_position(u3, geometry::point<double>(5000, 1000));

    interconnection::rc_tree rc_tree;
    timingdriven_placement::flute_rc_tree_creator flute;
    auto tap_mapping = flute.create_tree(placement, net, rc_tree, timing_library);


    REQUIRE( lemon::connected(rc_tree.graph()) );


    REQUIRE( lemon::countOutArcs(rc_tree.graph(), tap_mapping.at(u1o)) == 1 );
    auto u1o_cap = rc_tree.other_capacitor(rc_tree.capacitor_resistors(tap_mapping.at(u1o)), tap_mapping.at(u1o));
    REQUIRE( compare(rc_tree.capacitance(u1o_cap), quantity<capacitance>(0.1*femto*farad)) );

    REQUIRE( lemon::countOutArcs(rc_tree.graph(), tap_mapping.at(u2a)) == 1 );
    auto u2a_cap = rc_tree.other_capacitor(rc_tree.capacitor_resistors(tap_mapping.at(u2a)), tap_mapping.at(u2a));
    REQUIRE( compare(rc_tree.capacitance(u2a_cap), quantity<capacitance>(0.2*femto*farad)) );

    REQUIRE( lemon::countOutArcs(rc_tree.graph(), tap_mapping.at(u3a)) == 1 );
    auto u3a_cap = rc_tree.other_capacitor(rc_tree.capacitor_resistors(tap_mapping.at(u3a)), tap_mapping.at(u3a));
    REQUIRE( compare(rc_tree.capacitance(u3a_cap), quantity<capacitance>(0.3*femto*farad)) );


    REQUIRE( rc_tree.capacitor_count() == 7 );
    REQUIRE( rc_tree.capacitance(tap_mapping.at(u2a)) == timing_library.pin_capacitance( netlist.pin_std_cell(u2a) ) );
    REQUIRE( rc_tree.capacitance(tap_mapping.at(u1o)) == timing_library.pin_capacitance( netlist.pin_std_cell(u1o) ) );
    REQUIRE( rc_tree.capacitance(tap_mapping.at(u3a)) == timing_library.pin_capacitance( netlist.pin_std_cell(u3a) ) );
    REQUIRE( compare(rc_tree.lumped(), quantity<capacitance>(1.2*femto*farads)) );

}





TEST_CASE("flute rc_tree/tap mapping", "[tdp][flute][rc_tree]")
{
    standard_cell::standard_cells std_cells;

    netlist::netlist netlist{&std_cells};
	auto n1 = netlist.net_insert("n1");
	auto u1 = netlist.cell_insert("u1", "INV_X1");
	auto u2 = netlist.cell_insert("u2", "INV_X1");
	auto u3 = netlist.cell_insert("u3", "INV_X1");
	auto u4 = netlist.cell_insert("u4", "INV_X1");
	auto u1o = netlist.pin_insert(u1, "o");
	auto u2a = netlist.pin_insert(u2, "a");
	auto u3a = netlist.pin_insert(u3, "a");
	auto u4a = netlist.pin_insert(u4, "a");

    timing::library_timing_arcs tarcs{&std_cells};
    timing::library timing_library{&tarcs, &std_cells};

	netlist.connect(n1, u1o);
	netlist.connect(n1, u2a);
	netlist.connect(n1, u3a);
	netlist.connect(n1, u4a);

    timing_library.pin_capacitance( netlist.pin_std_cell(u2a), quantity<capacitance>(0*femto*farad) );

    entity::entity net{n1};
    placement::library library{&std_cells};
    library.dist2microns(1000);
    placement::placement placement{&netlist, &library};

    placement.cell_position(u1, geometry::point<double>(2000, 2000));
    placement.cell_position(u2, geometry::point<double>(3000, 4000));
    placement.cell_position(u3, geometry::point<double>(5000, 1000));
    placement.cell_position(u4, geometry::point<double>(5000, 5000));

    interconnection::rc_tree rc_tree;
    timingdriven_placement::flute_rc_tree_creator flute;
    auto tap_mapping = flute.create_tree(placement, net, rc_tree, timing_library);


    REQUIRE( lemon::connected(rc_tree.graph()) );


    REQUIRE( lemon::countOutArcs(rc_tree.graph(), tap_mapping.at(u1o)) == 1 );
    auto u1o_cap = rc_tree.other_capacitor(rc_tree.capacitor_resistors(tap_mapping.at(u1o)), tap_mapping.at(u1o));
    REQUIRE( compare(rc_tree.capacitance(u1o_cap), quantity<capacitance>(0.3*femto*farad)) );

    REQUIRE( lemon::countOutArcs(rc_tree.graph(), tap_mapping.at(u2a)) == 1 );
    auto u2a_cap = rc_tree.other_capacitor(rc_tree.capacitor_resistors(tap_mapping.at(u2a)), tap_mapping.at(u2a));
    REQUIRE( compare(rc_tree.capacitance(u2a_cap), quantity<capacitance>(0.2*femto*farad)) );


    REQUIRE( lemon::countOutArcs(rc_tree.graph(), tap_mapping.at(u3a)) == 1 );
    auto u3a_cap = rc_tree.other_capacitor(rc_tree.capacitor_resistors(tap_mapping.at(u3a)), tap_mapping.at(u3a));
    REQUIRE( compare(rc_tree.capacitance(u3a_cap), quantity<capacitance>(0.1*femto*farad)) );


    REQUIRE( lemon::countOutArcs(rc_tree.graph(), tap_mapping.at(u4a)) == 1 );
    auto u4a_cap = rc_tree.other_capacitor(rc_tree.capacitor_resistors(tap_mapping.at(u4a)), tap_mapping.at(u4a));
    REQUIRE( compare(rc_tree.capacitance(u4a_cap), quantity<capacitance>(0.1*femto*farad)) );


	REQUIRE( rc_tree.capacitor_count() == 10 );
	REQUIRE( rc_tree.capacitance(tap_mapping.at(u2a)) == timing_library.pin_capacitance( netlist.pin_std_cell(u2a) ) );
	REQUIRE( rc_tree.capacitance(tap_mapping.at(u1o)) == timing_library.pin_capacitance( netlist.pin_std_cell(u1o) ) );
    REQUIRE( compare(rc_tree.lumped(), quantity<capacitance>(1.8*femto*farads)) );

}


