#include "../catch.hpp"

#include "../interconnection/rc_tree.h"
#include <boost/units/systems/si.hpp>
#include <boost/units/systems/si/io.hpp>
#include <boost/units/systems/si/prefixes.hpp>

TEST_CASE("rc_tree/empty", "[timing][rc_tree]")
{
    openeda::interconnection::rc_tree tree;

    using namespace boost::units;
    using namespace boost::units::si;

    quantity<capacitance> zero_farad(0.0 * farads);

    REQUIRE( (tree.lumped() == zero_farad) );
    REQUIRE( tree.capacitor_count() == 0 );

}

TEST_CASE("rc_tree/insert node", "[timing][rc_tree]")
{
    openeda::interconnection::rc_tree tree;

    using namespace boost::units;
    using namespace boost::units::si;

    auto node = tree.capacitor_insert("u1");
    REQUIRE( tree.capacitance(node) == 0.0*farad );
    REQUIRE( tree.capacitor_count() == 1 );
}


TEST_CASE("rc_tree/set node capacitance", "[timing][rc_tree]")
{
    openeda::interconnection::rc_tree tree;

    using namespace boost::units;
    using namespace boost::units::si;

    auto node = tree.capacitor_insert("u1");
    tree.capacitance(node, quantity<capacitance>(1.3 * farad));

    REQUIRE( (tree.capacitance(node) == quantity<capacitance>(1.3 * farad)) );
    REQUIRE( (tree.lumped() == quantity<capacitance>(1.3 * farad) ) );

}

TEST_CASE("rc_tree/insert resistance", "[timing][rc_tree]")
{
    openeda::interconnection::rc_tree tree;

    using namespace boost::units;
    using namespace boost::units::si;

    auto u1o = tree.capacitor_insert("u1:o");
    auto in1 = tree.capacitor_insert("in1");

    auto res = tree.resistor_insert(u1o, in1, quantity<resistance>(1.1*kilo*ohms));
    REQUIRE( tree.resistance(res) == quantity<resistance>(1.1*kilo*ohms) );

}

TEST_CASE("rc_tree/get resistors connected to a capacitor", "[timing][rc_tree]")
{
    openeda::interconnection::rc_tree tree;
    using namespace boost::units;
    using namespace boost::units::si;
    auto u1o = tree.capacitor_insert("u1:o");
    auto in1 = tree.capacitor_insert("in1");
    REQUIRE( tree.capacitor_resistors(u1o) == openeda::interconnection::rc_tree::invalid() );
    auto res = tree.resistor_insert(u1o, in1, quantity<resistance>(1.1*kilo*ohms));
    bool found = false;
    for(openeda::interconnection::rc_tree::resistor_it it{tree.capacitor_resistors(u1o)}; it != openeda::interconnection::rc_tree::invalid(); ++it)
    {
        if(static_cast<openeda::interconnection::rc_tree::resistor_id>(it) == res)
            found = true;
    }
    REQUIRE( found );
}

TEST_CASE("rc_tree/get opposite capacitor of a resistor", "[timing][rc_tree]")
{
    openeda::interconnection::rc_tree tree;
    using namespace boost::units;
    using namespace boost::units::si;
    auto u1o = tree.capacitor_insert("u1:o");
    auto in1 = tree.capacitor_insert("in1");
    auto res = tree.resistor_insert(u1o, in1, quantity<resistance>(1.1*kilo*ohms));
    REQUIRE( tree.other_capacitor(res, u1o) == in1 );
}





