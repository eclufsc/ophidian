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
    std::shared_ptr<ophidian::parser::DesignConstraints> dc = ophidian::parser::SDCSimple().constraints();

    REQUIRE(dc->mClock.name == "mclk");
    REQUIRE(dc->mClock.period == 80.0);
    REQUIRE(dc->mClock.portName == "iccad_clk");

    REQUIRE(dc->mInputDelays[0].delay == 0.0);
    REQUIRE(dc->mInputDelays[0].portName == "inp1");
    REQUIRE(dc->mInputDelays[0].clock == "mclk");

    REQUIRE(dc->mInputDelays[1].delay == 0.0);
    REQUIRE(dc->mInputDelays[1].portName == "inp2");
    REQUIRE(dc->mInputDelays[1].clock == "mclk");

    REQUIRE(dc->mInputDrivers[0].libCell == "INV_X80");
    REQUIRE(dc->mInputDrivers[0].pinName == "o");
    REQUIRE(dc->mInputDrivers[0].portName == "inp1");
    REQUIRE(dc->mInputDrivers[0].slewFall == 10.0);
    REQUIRE(dc->mInputDrivers[0].slewRise == 10.0);

    REQUIRE(dc->mInputDrivers[1].libCell == "INV_X80");
    REQUIRE(dc->mInputDrivers[1].pinName == "o");
    REQUIRE(dc->mInputDrivers[1].portName == "inp2");
    REQUIRE(dc->mInputDrivers[1].slewFall == 10.0);
    REQUIRE(dc->mInputDrivers[1].slewRise == 10.0);

    REQUIRE(dc->mOutputDelays[0].delay == 0.0);
    REQUIRE(dc->mOutputDelays[0].portName == "out");
    REQUIRE(dc->mOutputDelays[0].clock == "mclk");

    REQUIRE(dc->mOutputLoads[0].pinLoad == 4.0);
    REQUIRE(dc->mOutputLoads[0].portName == "out");
}
