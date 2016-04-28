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
#include <boost/units/systems/si.hpp>
#include <boost/units/systems/si/io.hpp>
#include <boost/units/systems/si/prefixes.hpp>

TEST_CASE("rc_tree/empty", "[timing][rc_tree]")
{
    ophidian::interconnection::rc_tree tree;

    using namespace boost::units;
    using namespace boost::units::si;

    quantity<capacitance> zero_farad(0.0 * farads);

    REQUIRE( (tree.lumped() == zero_farad) );
    REQUIRE( tree.capacitor_count() == 0 );

}

TEST_CASE("rc_tree/insert node", "[timing][rc_tree]")
{
    ophidian::interconnection::rc_tree tree;

    using namespace boost::units;
    using namespace boost::units::si;

    auto node = tree.capacitor_insert("u1");
    REQUIRE( tree.capacitance(node) == 0.0*farad );
    REQUIRE( tree.capacitor_count() == 1 );
}


TEST_CASE("rc_tree/set node capacitance", "[timing][rc_tree]")
{
    ophidian::interconnection::rc_tree tree;

    using namespace boost::units;
    using namespace boost::units::si;

    auto node = tree.capacitor_insert("u1");
    tree.capacitance(node, quantity<capacitance>(1.3 * farad));

    REQUIRE( (tree.capacitance(node) == quantity<capacitance>(1.3 * farad)) );
    REQUIRE( (tree.lumped() == quantity<capacitance>(1.3 * farad) ) );

}

TEST_CASE("rc_tree/insert resistance", "[timing][rc_tree]")
{
    ophidian::interconnection::rc_tree tree;

    using namespace boost::units;
    using namespace boost::units::si;

    auto u1o = tree.capacitor_insert("u1:o");
    auto in1 = tree.capacitor_insert("in1");

    auto res = tree.resistor_insert(u1o, in1, quantity<resistance>(1.1*kilo*ohms));
    REQUIRE( tree.resistance(res) == quantity<resistance>(1.1*kilo*ohms) );

}

TEST_CASE("rc_tree/get resistors connected to a capacitor", "[timing][rc_tree]")
{
    ophidian::interconnection::rc_tree tree;
    using namespace boost::units;
    using namespace boost::units::si;
    auto u1o = tree.capacitor_insert("u1:o");
    auto in1 = tree.capacitor_insert("in1");
    REQUIRE( tree.capacitor_resistors(u1o) == ophidian::interconnection::rc_tree::invalid() );
    auto res = tree.resistor_insert(u1o, in1, quantity<resistance>(1.1*kilo*ohms));
    bool found = false;
    for(ophidian::interconnection::rc_tree::resistor_it it{tree.capacitor_resistors(u1o)}; it != ophidian::interconnection::rc_tree::invalid(); ++it)
    {
        if(static_cast<ophidian::interconnection::rc_tree::resistor_id>(it) == res)
            found = true;
    }
    REQUIRE( found );
}

TEST_CASE("rc_tree/get opposite capacitor of a resistor", "[timing][rc_tree]")
{
    ophidian::interconnection::rc_tree tree;
    using namespace boost::units;
    using namespace boost::units::si;
    auto u1o = tree.capacitor_insert("u1:o");
    auto in1 = tree.capacitor_insert("in1");
    auto res = tree.resistor_insert(u1o, in1, quantity<resistance>(1.1*kilo*ohms));
    REQUIRE( tree.other_capacitor(res, u1o) == in1 );
}



TEST_CASE("rc_tree/pack test", "[timing][rc_tree]")
{
    ophidian::interconnection::rc_tree tree;
    using namespace boost::units;
    using namespace boost::units::si;
    auto u1o = tree.capacitor_insert("u1:o");
    auto in1 = tree.capacitor_insert("in1");
    auto res = tree.resistor_insert(u1o, in1, quantity<resistance>(1.1*kilo*ohms));
    auto packed = tree.pack(u1o);
    REQUIRE( packed.node_count() == 2);
    REQUIRE( packed.pred(0) == std::numeric_limits<std::size_t>::max());
    REQUIRE( packed.pred(1) == 0);
    REQUIRE( packed.resistance(1) == quantity<resistance>(1.1*kilo*ohms));

}





