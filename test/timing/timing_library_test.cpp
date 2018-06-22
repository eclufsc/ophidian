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
Unless CHECKd by applicable law or agreed to in writing,
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

using liberty_type           = timing::Library::liberty_type;
using standard_cells_type    = timing::Library::standard_cells_type;
using timing_arcs_type       = timing::Library::timing_arcs_type;

using time_unit_type         = timing::Library::time_unit_type;
using capacitance_unit_type  = timing::Library::capacitance_unit_type;
using unateness_type         = timing::Library::unateness_type;
using timing_edge_type       = timing::Library::timing_edge_type;

using pin_entity_type        = timing::Library::pin_entity_type;
using std_pin_entity_type    = timing::Library::std_pin_entity_type;
using std_cell_entity_type   = timing::Library::std_cell_entity_type;
using timing_arc_entity_type = timing::Library::timing_arc_entity_type;

namespace
{
class LibraryFixture
{
public:
    standard_cells_type mStdCells;
    timing_arcs_type mArcs;
    std::shared_ptr<liberty_type> mLiberty;

    template<class Value>
    bool diff_E_max(const Value &a, const Value &b)
    {
        return units::math::abs(a - b) < Value(1.e+14);
    }

    template<class Value>
    bool diff_E_min(const Value &a, const Value &b)
    {
        return units::math::abs(a - b) < Value(1.e-14);
    }

    LibraryFixture() :
        mStdCells(),
        mLiberty(parser::LibertyParser().readFile("./input_files/sample2_Late.lib")),
        mArcs(mStdCells)
    {
        auto inv = mStdCells.add(std_cell_entity_type(), "INV_X1");
        auto invIn = mStdCells.add(std_pin_entity_type(), "INV_X1:a", standard_cell::PinDirection::INPUT);
        auto invOut = mStdCells.add(std_pin_entity_type(), "INV_X1:o", standard_cell::PinDirection::OUTPUT);
        mStdCells.add(inv, invIn);
        mStdCells.add(inv, invOut);

        auto ff = mStdCells.add(std_cell_entity_type(), "DFF_X80");
        auto ffCk = mStdCells.add(std_pin_entity_type(), "DFF_X80:ck", standard_cell::PinDirection::INOUT);
        auto ffIn = mStdCells.add(std_pin_entity_type(), "DFF_X80:d", standard_cell::PinDirection::INPUT);
        auto ffOut = mStdCells.add(std_pin_entity_type(), "DFF_X80:q", standard_cell::PinDirection::OUTPUT);
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
        CHECK(mArcs.size() == 0);
        timing::Library lib(*mLiberty.get(), mStdCells, mArcs, false);
        CHECK(mArcs.size() == 3);

        CHECK(!lib.cellSequential(mStdCells.find(std_cell_entity_type(), "INV_X1")));
        CHECK(!lib.pinClock(mStdCells.find(std_pin_entity_type(), "INV_X1:a")));
        CHECK(!lib.pinClock(mStdCells.find(std_pin_entity_type(), "INV_X1:o")));

        CHECK(lib.cellSequential(mStdCells.find(std_cell_entity_type(), "DFF_X80")));
        CHECK(lib.pinClock(mStdCells.find(std_pin_entity_type(), "DFF_X80:ck")));
        CHECK(!lib.pinClock(mStdCells.find(std_pin_entity_type(), "DFF_X80:d")));
        CHECK(!lib.pinClock(mStdCells.find(std_pin_entity_type(), "DFF_X80:q")));

        int i = 0;
        for (auto arcIt = mArcs.begin(); arcIt != mArcs.end(); arcIt++, i++)
        {
            auto arc = *arcIt;
            switch (i) {
            case 0:
                CHECK(mStdCells.name(mArcs.from(arc)) == "INV_X1:a");
                CHECK(mStdCells.name(mArcs.to(arc)) == "INV_X1:o");
                CHECK(lib.unateness(arc) == unateness_type::NEGATIVE_UNATE);
                CHECK(lib.type(arc) == timing_edge_type::COMBINATIONAL);
                CHECK(diff_E_max(lib.computeRiseDelay(arc, capacitance_unit_type(1.5),  time_unit_type(20.0)) , time_unit_type(1.1565e+14)));
                CHECK(diff_E_max(lib.computeFallDelay(arc, capacitance_unit_type(0.75), time_unit_type(325.0)), time_unit_type(9.39656e+14)));
                CHECK(diff_E_max(lib.computeRiseSlews(arc, capacitance_unit_type(18.5), time_unit_type(18.5)), time_unit_type(-1.36301e+15)));
                CHECK(diff_E_max(lib.computeFallSlews(arc, capacitance_unit_type(8.0),  time_unit_type(300.0)), time_unit_type(-9.558e+15)));
                CHECK(diff_E_min(lib.capacitance(mArcs.from(arc)), capacitance_unit_type(1e-15)));
                CHECK(lib.capacitance(mArcs.to(arc)) == capacitance_unit_type(0.0));
                break;
            case 1:
                CHECK(mStdCells.name(mArcs.from(arc)) == "DFF_X80:ck");
                CHECK(mStdCells.name(mArcs.to(arc)) == "DFF_X80:q");
                CHECK(lib.unateness(arc) == unateness_type::NON_UNATE);
                CHECK(lib.type(arc) == timing_edge_type::RISING_EDGE);
                CHECK(lib.computeRiseDelay(arc, capacitance_unit_type(128.0), time_unit_type(30.0)) == time_unit_type(6000));
                CHECK(lib.computeFallDelay(arc, capacitance_unit_type(2048.0), time_unit_type(300.0)) == time_unit_type(96000));
                CHECK(lib.computeRiseSlews(arc, capacitance_unit_type(512.0), time_unit_type(200.0)) == time_unit_type(24000));
                CHECK(lib.computeFallSlews(arc, capacitance_unit_type(32.0), time_unit_type(32.0)) == time_unit_type(1500));
                CHECK(diff_E_min(lib.capacitance(mArcs.from(arc)), capacitance_unit_type(1.5e-15)));
                CHECK(lib.capacitance(mArcs.to(arc)) == capacitance_unit_type(0.0));
                break;
            case 2:
                CHECK(mStdCells.name(mArcs.from(arc)) == "DFF_X80:ck");
                CHECK(mStdCells.name(mArcs.to(arc)) == "DFF_X80:d");
                CHECK(lib.type(arc) == timing_edge_type::SETUP_RISING);
                CHECK(diff_E_min(lib.computeRiseSlews(arc, capacitance_unit_type(1), time_unit_type(1)), time_unit_type(1.5e-12)));
                CHECK(diff_E_min(lib.computeFallSlews(arc, capacitance_unit_type(1), time_unit_type(1)), time_unit_type(2.5e-12)));
                CHECK(diff_E_min(lib.capacitance(mArcs.from(arc)), capacitance_unit_type(1.5e-15)));
                CHECK(diff_E_min(lib.capacitance(mArcs.to(arc)), capacitance_unit_type(3.49e-15)));
                break;
            default:
                break;
            }
        }
    }

    SECTION("Library: info about timing arcs in early mode", "[timing][library]")
    {
        CHECK(mArcs.size() == 0);
        timing::Library lib(*mLiberty.get(), mStdCells, mArcs, true);
        CHECK(mArcs.size() == 3);

        int i = 0;
        for (auto arcIt = mArcs.begin(); arcIt != mArcs.end(); arcIt++, i++)
        {
            auto arc = *arcIt;
            switch (i) {
            case 0:
                CHECK(mStdCells.name(mArcs.from(arc)) == "INV_X1:a");
                CHECK(mStdCells.name(mArcs.to(arc)) == "INV_X1:o");
                CHECK(lib.unateness(arc) == unateness_type::NEGATIVE_UNATE);
                CHECK(lib.type(arc) == timing_edge_type::COMBINATIONAL);
                CHECK(diff_E_max(lib.computeRiseDelay(arc, capacitance_unit_type(1.5), time_unit_type(20.0)), time_unit_type(1.1565e+14)));
                CHECK(diff_E_max(lib.computeFallDelay(arc, capacitance_unit_type(0.75), time_unit_type(325.0)), time_unit_type(9.39656e+14)));
                CHECK(diff_E_max(lib.computeRiseSlews(arc, capacitance_unit_type(18.5), time_unit_type(18.5)), time_unit_type(-1.36301e+15)));
                CHECK(diff_E_max(lib.computeFallSlews(arc, capacitance_unit_type(8.0), time_unit_type(300.0)), time_unit_type(-9.558e+15)));
                break;
            case 1:
                CHECK(mStdCells.name(mArcs.from(arc)) == "DFF_X80:ck");
                CHECK(mStdCells.name(mArcs.to(arc)) == "DFF_X80:q");
                CHECK(lib.unateness(arc) == unateness_type::NON_UNATE);
                CHECK(lib.type(arc) == timing_edge_type::RISING_EDGE);
                CHECK(lib.computeRiseDelay(arc, capacitance_unit_type(128.0), time_unit_type(30.0)) == time_unit_type(6000));
                CHECK(lib.computeFallDelay(arc, capacitance_unit_type(2048.0), time_unit_type(300.0)) == time_unit_type(96000));
                CHECK(lib.computeRiseSlews(arc, capacitance_unit_type(512.0), time_unit_type(200.0)) == time_unit_type(24000));
                CHECK(lib.computeFallSlews(arc, capacitance_unit_type(32.0), time_unit_type(32.0)) == time_unit_type(1500));
                break;
            case 2:
                CHECK(mStdCells.name(mArcs.from(arc)) == "DFF_X80:ck");
                CHECK(mStdCells.name(mArcs.to(arc)) == "DFF_X80:d");
                CHECK(lib.type(arc) == timing_edge_type::HOLD_RISING);
                CHECK(diff_E_min(lib.computeRiseSlews(arc, capacitance_unit_type(1), time_unit_type(1)), time_unit_type(3.5e-12)));
                CHECK(diff_E_min(lib.computeFallSlews(arc, capacitance_unit_type(1), time_unit_type(1)), time_unit_type(4.5e-12)));
                break;
            default:
                break;
            }
        }
    }
}


