/*
 * Copyright 2016 Ophidian
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
 */

#include "../catch.hpp"

#include "../interconnection/rc_tree.h"
#include "../timing/elmore.h"

#include <boost/units/systems/si/prefixes.hpp>

TEST_CASE("elmore_delay/rc_tree with 1 node", "[timing][rc_tree][elmore]")
{
    ophidian::interconnection::rc_tree tree;
    auto cap = tree.capacitor_insert("cap1");
    ophidian::timing::elmore delay(tree, cap);
    REQUIRE( delay.at(cap) == boost::units::quantity<boost::units::si::time>(0.0*boost::units::si::seconds) );
}

TEST_CASE("elmore_delay/rc_tree with 2 node2", "[timing][rc_tree][elmore]")
{
    using namespace boost::units;
    ophidian::interconnection::rc_tree tree;
    auto C0 = tree.capacitor_insert("C0");
    auto C1 = tree.capacitor_insert("C1");
    auto R1 = tree.resistor_insert(C0, C1, quantity<si::resistance>(0.223*si::kilo*si::ohms));
    tree.capacitance(C0, quantity<si::capacitance>(1.1*si::femto*si::farads));
    tree.capacitance(C1, quantity<si::capacitance>(2.2*si::femto*si::farads));
    ophidian::timing::elmore delay(tree, C0);
    REQUIRE( delay.at(C1) == quantity<si::time>((0.223*2.2)*si::pico*si::seconds) );
}

TEST_CASE("elmore_delay/simulating tap node", "[timing][rc_tree][elmore]")
{
    using namespace boost::units;
    ophidian::interconnection::rc_tree tree;
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

    ophidian::timing::elmore delay(tree, C0);
    quantity<si::time> golden_delay{tree.resistance(R2)*(tree.capacitance(u1_a_tap)+tree.capacitance(u1_a))+tree.resistance(R1)*(tree.capacitance(u1_a_tap)+tree.capacitance(u1_a)+tree.capacitance(C1))};
    REQUIRE( delay.at(u1_a) == golden_delay );
}
