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

#include <ophidian/timing/Elmore.h>

TEST_CASE("Elmore Delay: RCTree with 1 node", "[timing][RCTree][Elmore]")
{
    ophidian::timingdriven_placement::RCTree rctree;

    auto cap = rctree.addCapacitor("cap1");

    ophidian::timing::Elmore delay(rctree, cap);

    REQUIRE(delay.at(cap) == ophidian::util::second_t(0.0));
}

TEST_CASE("Elmore Delay: RCTree with 2 nodes", "[timing][RCTree][Elmore]")
{
    ophidian::timingdriven_placement::RCTree rctree;

    auto C0 = rctree.addCapacitor("C0");
    auto C1 = rctree.addCapacitor("C1");
    auto R1 = rctree.addResistor(C0, C1, ophidian::util::ohm_t(ophidian::util::kiloohm_t(0.223)));
    rctree.capacitance(C0, ophidian::util::farad_t(ophidian::util::femtofarad_t(1.1)));
    rctree.capacitance(C1, ophidian::util::farad_t(ophidian::util::femtofarad_t(2.2)));

    ophidian::timing::Elmore delay(rctree, C0);

    REQUIRE(delay.at(C1) == ophidian::util::second_t(ophidian::util::picosecond_t((0.223 * 2.2))));
}

TEST_CASE("Elmore Delay: Simulating tap node", "[timing][RCTree][Elmore]")
{
    /*

        R1       R2       R3 = 0ohms (virtual resistance)
    _/\/\/\___/\/\/\___/\/\/\__
    |        |        |        |
    = c0     = c1     = u1:a   = u1:a
    |        |        | (int.) | (pin capacitance)
    z        z        z        z


    */

    ophidian::timingdriven_placement::RCTree rctree;

    auto C0 = rctree.addCapacitor("C0"); // internal node
    rctree.capacitance(C0, ophidian::util::farad_t(ophidian::util::femtofarad_t(0.20)));

    auto C1 = rctree.addCapacitor("C1"); // internal node
    rctree.capacitance(C1, ophidian::util::farad_t(ophidian::util::femtofarad_t(0.20)));

    auto u1_a_tap = rctree.addCapacitor("u1:a_tap"); // tap node
    rctree.capacitance(u1_a_tap, ophidian::util::farad_t(ophidian::util::femtofarad_t(0.20)));

    auto u1_a = rctree.addCapacitor("u1:a"); // pin capacitance
    rctree.capacitance(u1_a, ophidian::util::farad_t(ophidian::util::femtofarad_t(4.0)));

    auto R1 = rctree.addResistor(C0, C1, ophidian::util::ohm_t(ophidian::util::kiloohm_t(0.440)));       // internal resistor
    auto R2 = rctree.addResistor(C1, u1_a_tap, ophidian::util::ohm_t(ophidian::util::kiloohm_t(0.440))); // internal resistor
    auto R3 = rctree.addResistor(u1_a_tap, u1_a, ophidian::util::ohm_t(0.0));                            // virtual resistor

    ophidian::timing::Elmore delay(rctree, C0);
    ophidian::util::second_t golden_delay(rctree.resistance(R2) * (rctree.capacitance(u1_a_tap) + rctree.capacitance(u1_a)) +
                                          rctree.resistance(R1) * (rctree.capacitance(u1_a_tap) + rctree.capacitance(u1_a) + rctree.capacitance(C1)));

    REQUIRE(delay.at(u1_a) == golden_delay);
}
