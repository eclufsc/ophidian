/*
 * Copyright 2017 Ophidian
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

#include <catch.hpp>

#include <ophidian/timingdriven_placement/RCTree.h>

using namespace ophidian::timingdriven_placement;

TEST_CASE("RCTree: empty RCTree", "[timingdriven_placement][RCTree]")
{
    RCTree tree;
    REQUIRE(tree.size(RCTree::capacitor_type()) == 0);
    REQUIRE(tree.size(RCTree::resistor_type()) == 0);
    REQUIRE(tree.lumped() == ophidian::util::farad_t(0.0));
}

TEST_CASE("RCTree: add capacitor_type", "[timingdriven_placement][RCTree]")
{
    RCTree tree;
    tree.addCapacitor("cap");
    REQUIRE(tree.size(RCTree::capacitor_type()) == 1);
    REQUIRE(tree.size(RCTree::resistor_type()) == 0);
    REQUIRE(tree.lumped() == ophidian::util::farad_t(0.0));
}

TEST_CASE("RCTree: add Capacitance of the capacitor", "[timingdriven_placement][RCTree]")
{
    RCTree tree;
    auto cap = tree.addCapacitor("cap");
    tree.capacitance(cap, ophidian::util::farad_t(1.0));
    REQUIRE(tree.size(RCTree::capacitor_type()) == 1);
    REQUIRE(tree.lumped() == ophidian::util::farad_t(1.0));
}

TEST_CASE("RCTree: add the same capacitor twice", "[timingdriven_placement][RCTree]")
{
    RCTree tree;
    tree.addCapacitor("cap");
    tree.addCapacitor("cap");
    REQUIRE(tree.size(RCTree::capacitor_type()) == 1);
}

TEST_CASE("RCTree: add resistor_type", "[timingdriven_placement][RCTree]")
{
    RCTree tree;
    auto capU = tree.addCapacitor("capU");
    auto capV = tree.addCapacitor("capV");
    auto res = tree.addResistor(capU, capV, ophidian::util::ohm_t(1.1));
    REQUIRE(tree.size(RCTree::capacitor_type()) == 2);
    REQUIRE(tree.size(RCTree::resistor_type()) == 1);
    REQUIRE(tree.resistance(res) == ophidian::util::ohm_t(1.1));
}

TEST_CASE("RCTree: name of capacitor", "[timingdriven_placement][RCTree]")
{
    RCTree tree;
    auto cap = tree.addCapacitor("cap");
    REQUIRE(tree.name(cap) == "cap");
}

TEST_CASE("RCTree: find capacitor by name", "[timingdriven_placement][RCTree]")
{
    RCTree tree;
    auto cap = tree.addCapacitor("cap");
    REQUIRE(tree.capacitor("cap") == cap);
}

TEST_CASE("RCTree: find resistor by two capacitors", "[timingdriven_placement][RCTree]")
{
    RCTree tree;
    auto capU = tree.addCapacitor("capU");
    auto capV = tree.addCapacitor("capV");
    auto res = tree.addResistor(capU, capV, ophidian::util::ohm_t(1.1));
    REQUIRE(tree.g().id(tree.resistor(capU, capV)) == tree.g().id(res));
    REQUIRE(tree.g().id(tree.resistor(capV, capU)) == tree.g().id(res));
    REQUIRE(tree.resistor(capU, RCTree::capacitor_type()) == lemon::INVALID);
    REQUIRE(tree.resistor(RCTree::capacitor_type(), capV) == lemon::INVALID);
}

TEST_CASE("RCTree: find opposite capacitor", "[timingdriven_placement][RCTree]")
{
    RCTree tree;
    auto capU = tree.addCapacitor("capU");
    auto capV = tree.addCapacitor("capV");
    auto res = tree.addResistor(capU, capV, ophidian::util::ohm_t(1.1));
    REQUIRE(tree.oppositeCapacitor(capU, res) == capV);
    REQUIRE(tree.oppositeCapacitor(capV, res) == capU);
}

TEST_CASE("RCTree: resitor iterator", "[timingdriven_placement][RCTree]")
{
    RCTree tree;
    auto capU = tree.addCapacitor("capU");
    auto capV = tree.addCapacitor("capV");
    auto res = tree.addResistor(capU, capV, ophidian::util::ohm_t(1.1));

    REQUIRE(tree.oppositeCapacitor(capU, tree.resistors(capU)) == capV);
    REQUIRE(tree.oppositeCapacitor(capV, tree.resistors(capV)) == capU);
    REQUIRE(tree.invalid() == lemon::INVALID);
}

TEST_CASE("RCTree: Predecessors", "[timingdriven_placement][RCTree]")
{
    //  s -> a -> c
    //    -> b -> c

    RCTree tree;
    auto s = tree.addCapacitor("s");
    auto a = tree.addCapacitor("a");
    auto b = tree.addCapacitor("b");
    auto c = tree.addCapacitor("c");
    auto s_a = tree.addResistor(s, a, ophidian::util::ohm_t(1.1));
    auto s_b = tree.addResistor(s, b, ophidian::util::ohm_t(1.1));
    auto a_c = tree.addResistor(a, c, ophidian::util::ohm_t(1.1));
    auto b_c = tree.addResistor(b, c, ophidian::util::ohm_t(1.1));

    tree.source(s);

    REQUIRE(tree.name(tree.pred(a)) == "s");
    REQUIRE(tree.name(tree.pred(b)) == "s");
    REQUIRE(tree.name(tree.pred(c)) == "b");
    CHECK_THROWS(tree.name(tree.pred(s)));
    CHECK_THROWS(tree.name(RCTree::capacitor_type()));
}
