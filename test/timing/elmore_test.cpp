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

TEST_CASE("Elmore Delay", "[timing][RCTree][Elmore]")
{
    using namespace ophidian;
    using time_unit_type = timing::Elmore::time_unit_type;
    using capacitance_unit_type = timingdriven_placement::RCTree::capacitance_unit_type;
    using resistance_unit_type  = timingdriven_placement::RCTree::resistance_unit_type;

    SECTION("Elmore Delay: RCTree with 1 node", "[timing][RCTree][Elmore]")
    {
        timingdriven_placement::RCTree rctree;

        auto cap = rctree.addCapacitor("cap1");
        rctree.source(cap);

        timing::Elmore delay(rctree);

        REQUIRE(delay.at(cap) == time_unit_type(0.0));
    }

    SECTION("Elmore Delay: RCTree with 2 nodes", "[timing][RCTree][Elmore]")
    {
        timingdriven_placement::RCTree rctree;

        auto C0 = rctree.addCapacitor("C0");
        auto C1 = rctree.addCapacitor("C1");
        rctree.source(C0);

        auto R1 = rctree.addResistor(C0, C1, resistance_unit_type(util::kiloohm_t(0.223)));
        rctree.capacitance(C0, capacitance_unit_type(util::femtofarad_t(1.1)));
        rctree.capacitance(C1, capacitance_unit_type(util::femtofarad_t(2.2)));

        timing::Elmore delay(rctree);

        REQUIRE(delay.at(C1) == time_unit_type(util::picosecond_t((0.223 * 2.2))));
    }

    SECTION("Elmore Delay: Simulating tap node", "[timing][RCTree][Elmore]")
    {
        /*

            R1       R2       R3 = 0ohms (virtual resistance)
        _/\/\/\___/\/\/\___/\/\/\__
        |        |        |        |
        = c0     = c1     = u1:a   = u1:a
        |        |        | (int.) | (pin capacitance)
        z        z        z        z


        */

        timingdriven_placement::RCTree rctree;

        auto C0 = rctree.addCapacitor("C0");
        rctree.source(C0);
        rctree.capacitance(C0, capacitance_unit_type(util::femtofarad_t(0.20)));

        auto C1 = rctree.addCapacitor("C1");
        rctree.capacitance(C1, capacitance_unit_type(util::femtofarad_t(0.20)));

        auto u1_a_tap = rctree.addCapacitor("u1:a_tap");
        rctree.capacitance(u1_a_tap, capacitance_unit_type(util::femtofarad_t(0.20)));

        auto u1_a = rctree.addCapacitor("u1:a");
        rctree.capacitance(u1_a, capacitance_unit_type(util::femtofarad_t(4.0)));

        auto R1 = rctree.addResistor(C0, C1, resistance_unit_type(util::kiloohm_t(0.440)));
        auto R2 = rctree.addResistor(C1, u1_a_tap, resistance_unit_type(util::kiloohm_t(0.440)));
        auto R3 = rctree.addResistor(u1_a_tap, u1_a, resistance_unit_type(0.0));

        timing::Elmore delay(rctree);
        time_unit_type golden_delay(
                    rctree.resistance(R2) * (rctree.capacitance(u1_a_tap) + rctree.capacitance(u1_a)) +
                    rctree.resistance(R1) * (rctree.capacitance(u1_a_tap) + rctree.capacitance(u1_a) + rctree.capacitance(C1)));

        REQUIRE(delay.at(u1_a) == golden_delay);
    }
}

