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

#include "../ophidian/interconnection/RCTree.h"
#include <boost/units/systems/si.hpp>
#include <boost/units/systems/si/io.hpp>
#include <boost/units/systems/si/prefixes.hpp>

TEST_CASE("RCTree/empty", "[timing][interconnection]")
{
    ophidian::interconnection::RCTree tree;

    using namespace boost::units;
    using namespace boost::units::si;

    quantity<capacitance> zero_farad(0.0 * farads);

    REQUIRE( (tree.lumped() == zero_farad) );
    REQUIRE( tree.capacitorCount() == 0 );

}

TEST_CASE("RCTree/insert node", "[timing][interconnection]")
{
    ophidian::interconnection::RCTree tree;

    using namespace boost::units;
    using namespace boost::units::si;

    auto node = tree.capacitorInsert("u1");
    REQUIRE( tree.capacitance(node) == 0.0*farad );
    REQUIRE( tree.capacitorCount() == 1 );
}


TEST_CASE("RCTree/set node capacitance", "[timing][interconnection]")
{
    ophidian::interconnection::RCTree tree;

    using namespace boost::units;
    using namespace boost::units::si;

    auto node = tree.capacitorInsert("u1");
    tree.capacitance(node, quantity<capacitance>(1.3 * farad));

    REQUIRE( (tree.capacitance(node) == quantity<capacitance>(1.3 * farad)) );
    REQUIRE( (tree.lumped() == quantity<capacitance>(1.3 * farad) ) );

}

TEST_CASE("RCTree/insert resistance", "[timing][interconnection]")
{
    ophidian::interconnection::RCTree tree;

    using namespace boost::units;
    using namespace boost::units::si;

    auto u1o = tree.capacitorInsert("u1:o");
    auto in1 = tree.capacitorInsert("in1");

    auto res = tree.resistorInsert(u1o, in1, quantity<resistance>(1.1*kilo*ohms));
    REQUIRE( tree.resistance(res) == quantity<resistance>(1.1*kilo*ohms) );

}

TEST_CASE("RCTree/get resistors connected to a capacitor", "[timing][interconnection]")
{
    ophidian::interconnection::RCTree tree;
    using namespace boost::units;
    using namespace boost::units::si;
    auto u1o = tree.capacitorInsert("u1:o");
    auto in1 = tree.capacitorInsert("in1");
    REQUIRE( tree.capacitorResistors(u1o) == ophidian::interconnection::RCTree::invalid() );
    auto res = tree.resistorInsert(u1o, in1, quantity<resistance>(1.1*kilo*ohms));
    bool found = false;
    for(ophidian::interconnection::RCTree::resistorIt it{tree.capacitorResistors(u1o)}; it != ophidian::interconnection::RCTree::invalid(); ++it)
    {
        if(static_cast<ophidian::interconnection::RCTree::resistorID>(it) == res)
            found = true;
    }
    REQUIRE( found );
}

TEST_CASE("RCTree/get opposite capacitor of a resistor", "[timing][interconnection]")
{
    ophidian::interconnection::RCTree tree;
    using namespace boost::units;
    using namespace boost::units::si;
    auto u1o = tree.capacitorInsert("u1:o");
    auto in1 = tree.capacitorInsert("in1");
    auto res = tree.resistorInsert(u1o, in1, quantity<resistance>(1.1*kilo*ohms));
    REQUIRE( tree.otherCapacitor(res, u1o) == in1 );
}



TEST_CASE("RCTree/pack test", "[timing][interconnection]")
{
    ophidian::interconnection::RCTree tree;
    using namespace boost::units;
    using namespace boost::units::si;
    auto u1o = tree.capacitorInsert("u1:o");
    auto in1 = tree.capacitorInsert("in1");
    auto res = tree.resistorInsert(u1o, in1, quantity<resistance>(1.1*kilo*ohms));
    auto packed = tree.pack(u1o);
    REQUIRE( packed.nodeCount() == 2);
    REQUIRE( packed.pred(0) == std::numeric_limits<std::size_t>::max());
    REQUIRE( packed.pred(1) == 0);
    REQUIRE( packed.resistance(1) == quantity<resistance>(1.1*kilo*ohms));

}

TEST_CASE("RCTree/simple's n3 net test", "[timing][interconnection]")
{
    ophidian::interconnection::RCTree tree;
    using namespace boost::units;
    using namespace boost::units::si;
    auto f1q = tree.capacitorInsert("f1:q");
    auto u2b = tree.capacitorInsert("u2:b");
    auto u3a = tree.capacitorInsert("u3:a");
    tree.capacitance(f1q, quantity<capacitance>(0.1964 * femto * farad));
    tree.capacitance(u2b, quantity<capacitance>(0.2902 * femto * farad));
    tree.capacitance(u3a, quantity<capacitance>(0.0029723 * femto * farad));
    tree.resistorInsert(f1q, u2b, quantity<resistance>( 0.0062234 * kilo * ohms));
    tree.resistorInsert(u3a, u2b, quantity<resistance>( 0.0029723 * kilo * ohms));
    REQUIRE(tree.capacitorCount() == 3);
    REQUIRE(tree.lumped() == quantity<capacitance>((0.1964 + 0.2902 + 0.0029723) * femto * farad));
}
