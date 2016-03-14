#include "../catch.hpp"

#include "../timing/rc_tree.h"
#include "../timing/elmore.h"

#include <boost/units/systems/si/prefixes.hpp>

TEST_CASE("elmore_delay/rc_tree with 1 node", "[timing][elmore]")
{
	openeda::timing::rc_tree tree;
    auto cap = tree.capacitor_insert("cap1");
	openeda::timing::elmore delay(tree, cap);
	REQUIRE( delay.get(cap) == boost::units::quantity<boost::units::si::time>(0.0*boost::units::si::seconds) );
}

TEST_CASE("elmore_delay/rc_tree with 2 node2", "[timing][elmore]")
{
    using namespace boost::units;
    openeda::timing::rc_tree tree;
    auto C0 = tree.capacitor_insert("C0");
    auto C1 = tree.capacitor_insert("C1");
    auto R1 = tree.resistor_insert(C0, C1, quantity<si::resistance>(0.223*si::kilo*si::ohms));
    tree.capacitance(C0, quantity<si::capacitance>(1.1*si::femto*si::farads));
    tree.capacitance(C1, quantity<si::capacitance>(2.2*si::femto*si::farads));
    openeda::timing::elmore delay(tree, C0);
    REQUIRE( delay.get(C1) == quantity<si::time>((0.223*2.2)*si::pico*si::seconds) );
}

TEST_CASE("elmore_delay/simulating tap node", "[timing][elmore]")
{
    using namespace boost::units;
    openeda::timing::rc_tree tree;
    /*

    R1       R2       R3 = 0ohms (virtual resistance)
 _/\/\/\___/\/\/\___/\/\/\__
|        |        |        |
= c0     = c1     = u1:a   = u1:a
|        |        | (int.) | (pin capacitance)
z        z        z        z


    */

    auto C0 = tree.capacitor_insert("C0"); // internal node
    tree.capacitance(C0, quantity<si::capacitance>(0.20*si::femto*si::farads));
    auto C1 = tree.capacitor_insert("C1"); // internal node
    tree.capacitance(C1, quantity<si::capacitance>(0.20*si::femto*si::farads));
    auto u1_a_tap = tree.capacitor_insert("u1:a_tap"); // tap node
    tree.capacitance(u1_a_tap, quantity<si::capacitance>(0.20*si::femto*si::farads));
    auto u1_a = tree.capacitor_insert("u1:a"); // pin capacitance
    tree.capacitance(u1_a, quantity<si::capacitance>(4.0*si::femto*si::farads));

    auto R1 = tree.resistor_insert(C0, C1, quantity<si::resistance>(0.440*si::kilo*si::ohms)); // internal resistor
    auto R2 = tree.resistor_insert(C1, u1_a_tap, quantity<si::resistance>(0.440*si::kilo*si::ohms)); // internal resistor
    auto R3 = tree.resistor_insert(u1_a_tap, u1_a, quantity<si::resistance>(0.0*si::ohms)); // virtual resistor

    openeda::timing::elmore delay(tree, C0);
    quantity<si::time> golden_delay{tree.resistance(R2)*(tree.capacitance(u1_a_tap)+tree.capacitance(u1_a))+tree.resistance(R1)*(tree.capacitance(u1_a_tap)+tree.capacitance(u1_a)+tree.capacitance(C1))};
    REQUIRE( delay.get(u1_a) == golden_delay );
}
