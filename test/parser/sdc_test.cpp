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

#include <ophidian/parser/SDCParser.h>

TEST_CASE("DesignConstrains: Simple DC", "[parser][sdc][DesignConstrains]")
{
    auto dc = ophidian::parser::SDCSimple().constraints();

    REQUIRE(dc->m_clock.name == "mclk");
    REQUIRE(dc->m_clock.period == 80.0);
    REQUIRE(dc->m_clock.port_name == "iccad_clk");

    REQUIRE(dc->m_input_delays[0].delay == 0.0);
    REQUIRE(dc->m_input_delays[0].port_name == "inp1");
    REQUIRE(dc->m_input_delays[0].clock == "mclk");

    REQUIRE(dc->m_input_delays[1].delay == 0.0);
    REQUIRE(dc->m_input_delays[1].port_name == "inp2");
    REQUIRE(dc->m_input_delays[1].clock == "mclk");

    REQUIRE(dc->m_input_drivers[0].lib_cell == "INV_X80");
    REQUIRE(dc->m_input_drivers[0].pin_name == "o");
    REQUIRE(dc->m_input_drivers[0].port_name == "inp1");
    REQUIRE(dc->m_input_drivers[0].slew_fall == 10.0);
    REQUIRE(dc->m_input_drivers[0].slew_rise == 10.0);

    REQUIRE(dc->m_input_drivers[1].lib_cell == "INV_X80");
    REQUIRE(dc->m_input_drivers[1].pin_name == "o");
    REQUIRE(dc->m_input_drivers[1].port_name == "inp2");
    REQUIRE(dc->m_input_drivers[1].slew_fall == 10.0);
    REQUIRE(dc->m_input_drivers[1].slew_rise == 10.0);

    REQUIRE(dc->m_output_delays[0].delay == 0.0);
    REQUIRE(dc->m_output_delays[0].port_name == "out");
    REQUIRE(dc->m_output_delays[0].clock == "mclk");

    REQUIRE(dc->m_output_loads[0].pin_load == 4.0);
    REQUIRE(dc->m_output_loads[0].port_name == "out");
}
