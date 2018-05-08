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

#include <ophidian/util/Units.h>
#include <ophidian/timing/Library.h>

using namespace ophidian;

namespace
{
class LibraryFixture
{
public:
    standard_cell::StandardCells mStdCells;
    timing::TimingArcs mArcs;
    std::shared_ptr<parser::Liberty> mLiberty;

    LibraryFixture() :
        mStdCells(),
        mLiberty(parser::LibertyParser().readFile("./input_files/sample2_Late.lib")),
        mArcs(mStdCells)
    {
        auto inv = mStdCells.add(standard_cell::Cell(), "INV_X1");
        auto invIn = mStdCells.add(standard_cell::Pin(), "INV_X1:a", standard_cell::PinDirection::INPUT);
        auto invOut = mStdCells.add(standard_cell::Pin(), "INV_X1:o", standard_cell::PinDirection::OUTPUT);
        mStdCells.add(inv, invIn);
        mStdCells.add(inv, invOut);

        auto ff = mStdCells.add(standard_cell::Cell(), "DFF_X80");
        auto ffCk = mStdCells.add(standard_cell::Pin(), "DFF_X80:ck", standard_cell::PinDirection::INOUT);
        auto ffIn = mStdCells.add(standard_cell::Pin(), "DFF_X80:d", standard_cell::PinDirection::INPUT);
        auto ffOut = mStdCells.add(standard_cell::Pin(), "DFF_X80:q", standard_cell::PinDirection::OUTPUT);
        mStdCells.add(ff, ffCk);
        mStdCells.add(ff, ffIn);
        mStdCells.add(ff, ffOut);
    }
};
} // namespace

TEST_CASE_METHOD(LibraryFixture, "Library: init", "[timing][library]")
{
    SECTION("Library: info about timing arcs in late mode", "[timing][library]")
    {
        REQUIRE(mArcs.size() == 0);
        timing::Library lib(*mLiberty.get(), mStdCells, mArcs, false);
        REQUIRE(mArcs.size() == 3);

        REQUIRE(!lib.cellSequential(mStdCells.find(standard_cell::Cell(), "INV_X1")));
        REQUIRE(!lib.pinClock(mStdCells.find(standard_cell::Pin(), "INV_X1:a")));
        REQUIRE(!lib.pinClock(mStdCells.find(standard_cell::Pin(), "INV_X1:o")));

        REQUIRE(lib.cellSequential(mStdCells.find(standard_cell::Cell(), "DFF_X80")));
        REQUIRE(lib.pinClock(mStdCells.find(standard_cell::Pin(), "DFF_X80:ck")));
        REQUIRE(!lib.pinClock(mStdCells.find(standard_cell::Pin(), "DFF_X80:d")));
        REQUIRE(!lib.pinClock(mStdCells.find(standard_cell::Pin(), "DFF_X80:q")));

        int i = 0;
        for (auto arcIt = mArcs.begin(); arcIt != mArcs.end(); arcIt++, i++)
        {
            auto arc = *arcIt;
            switch (i) {
            case 0:
                REQUIRE(mStdCells.name(mArcs.from(arc)) == "INV_X1:a");
                REQUIRE(mStdCells.name(mArcs.to(arc)) == "INV_X1:o");
                REQUIRE(lib.unateness(arc) == timing::Library::unateness_type::NEGATIVE_UNATE);
                REQUIRE(lib.type(arc) == timing::Library::timing_edge_type::COMBINATIONAL);
                REQUIRE(lib.computeRiseDelay(arc, util::farad_t(1.5),  util::second_t(20.0))  == util::second_t(28.116));
                REQUIRE(lib.computeFallDelay(arc, util::farad_t(0.75), util::second_t(325.0)) == util::second_t(71.244375));
                REQUIRE(lib.computeRiseSlews(arc, util::farad_t(18.5), util::second_t(18.5))  == util::second_t(153.75));
                REQUIRE(lib.computeFallSlews(arc, util::farad_t(8.0),  util::second_t(300.0)) == util::second_t(106.536));
                REQUIRE(lib.capacitance(mArcs.from(arc)) == util::farad_t(1.0));
                REQUIRE(lib.capacitance(mArcs.to(arc)) == util::farad_t(0.0));
                break;
            case 1:
                REQUIRE(mStdCells.name(mArcs.from(arc)) == "DFF_X80:ck");
                REQUIRE(mStdCells.name(mArcs.to(arc)) == "DFF_X80:q");
                REQUIRE(lib.unateness(arc) == timing::Library::unateness_type::NON_UNATE);
                REQUIRE(lib.type(arc) == timing::Library::timing_edge_type::RISING_EDGE);
                REQUIRE(lib.computeRiseDelay(arc, util::farad_t(128.0), util::second_t(30.0)) == util::second_t(25.2));
                REQUIRE(lib.computeFallDelay(arc, util::farad_t(2048.0), util::second_t(300.0)) == util::second_t(115.2));
                REQUIRE(lib.computeRiseSlews(arc, util::farad_t(512.0), util::second_t(200.0)) == util::second_t(43.2));
                REQUIRE(lib.computeFallSlews(arc, util::farad_t(32.0), util::second_t(32.0)) == util::second_t(20.7));
                REQUIRE(lib.capacitance(mArcs.from(arc)) == util::farad_t(1.5));
                REQUIRE(lib.capacitance(mArcs.to(arc)) == util::farad_t(0.0));
                break;
            case 2:
                REQUIRE(mStdCells.name(mArcs.from(arc)) == "DFF_X80:ck");
                REQUIRE(mStdCells.name(mArcs.to(arc)) == "DFF_X80:d");
                REQUIRE(lib.type(arc) == timing::Library::timing_edge_type::SETUP_RISING);
                REQUIRE(lib.computeRiseSlews(arc, util::farad_t(1), util::second_t(1)) == util::second_t(1.5));
                REQUIRE(lib.computeFallSlews(arc, util::farad_t(1), util::second_t(1)) == util::second_t(2.5));
                REQUIRE(lib.capacitance(mArcs.from(arc)) ==  util::farad_t(1.5));
                REQUIRE(lib.capacitance(mArcs.to(arc)) ==  util::farad_t(3.49));
                break;
            default:
                break;
            }
        }
    }

    SECTION("Library: info about timing arcs in early mode", "[timing][library]")
    {
        REQUIRE(mArcs.size() == 0);
        timing::Library lib(*mLiberty.get(), mStdCells, mArcs, true);
        REQUIRE(mArcs.size() == 3);

        int i = 0;
        for (auto arcIt = mArcs.begin(); arcIt != mArcs.end(); arcIt++, i++)
        {
            auto arc = *arcIt;
            switch (i) {
            case 0:
                REQUIRE(mStdCells.name(mArcs.from(arc)) == "INV_X1:a");
                REQUIRE(mStdCells.name(mArcs.to(arc)) == "INV_X1:o");
                REQUIRE(lib.unateness(arc) == timing::Library::unateness_type::NEGATIVE_UNATE);
                REQUIRE(lib.type(arc) == timing::Library::timing_edge_type::COMBINATIONAL);
                REQUIRE(lib.computeRiseDelay(arc, util::farad_t(1.5), util::second_t(20.0)) == util::second_t(28.116));
                REQUIRE(lib.computeFallDelay(arc, util::farad_t(0.75), util::second_t(325.0)) == util::second_t(71.244375));
                REQUIRE(lib.computeRiseSlews(arc, util::farad_t(18.5), util::second_t(18.5)) == util::second_t(153.75));
                REQUIRE(lib.computeFallSlews(arc, util::farad_t(8.0), util::second_t(300.0)) == util::second_t(106.536));
                break;
            case 1:
                REQUIRE(mStdCells.name(mArcs.from(arc)) == "DFF_X80:ck");
                REQUIRE(mStdCells.name(mArcs.to(arc)) == "DFF_X80:q");
                REQUIRE(lib.unateness(arc) == timing::Library::unateness_type::NON_UNATE);
                REQUIRE(lib.type(arc) == timing::Library::timing_edge_type::RISING_EDGE);
                REQUIRE(lib.computeRiseDelay(arc, util::farad_t(128.0), util::second_t(30.0)) == util::second_t(25.2));
                REQUIRE(lib.computeFallDelay(arc, util::farad_t(2048.0), util::second_t(300.0)) == util::second_t(115.2));
                REQUIRE(lib.computeRiseSlews(arc, util::farad_t(512.0), util::second_t(200.0)) == util::second_t(43.2));
                REQUIRE(lib.computeFallSlews(arc, util::farad_t(32.0), util::second_t(32.0)) == util::second_t(20.7));
                break;
            case 2:
                REQUIRE(mStdCells.name(mArcs.from(arc)) == "DFF_X80:ck");
                REQUIRE(mStdCells.name(mArcs.to(arc)) == "DFF_X80:d");
                REQUIRE(lib.type(arc) == timing::Library::timing_edge_type::HOLD_RISING);
                REQUIRE(lib.computeRiseSlews(arc, util::farad_t(1), util::second_t(1)) == util::second_t(3.5));
                REQUIRE(lib.computeFallSlews(arc, util::farad_t(1), util::second_t(1)) == util::second_t(4.5));
                break;
            default:
                break;
            }
        }
    }
}


