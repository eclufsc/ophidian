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
using pin_direction_type     = timing::Library::pin_direction_type;
using std_pin_entity_type    = timing::Library::std_pin_entity_type;
using std_cell_entity_type   = timing::Library::std_cell_entity_type;
using timing_arc_entity_type = timing::Library::timing_arc_entity_type;

namespace
{
class LibraryFixture
{
public:
    standard_cells_type m_std_cells;
    timing_arcs_type m_arcs;
    std::shared_ptr<liberty_type> m_liberty;

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
        m_std_cells(),
        m_arcs(m_std_cells),
        m_liberty(std::make_shared<parser::Liberty>(parser::Liberty("./input_files/simple/sample_Late.lib")))
    {
        auto inv = m_std_cells.add_cell("INV_X1");
        auto invIn = m_std_cells.add_pin("INV_X1:a", pin_direction_type::INPUT);
        auto invOut = m_std_cells.add_pin("INV_X1:o", pin_direction_type::OUTPUT);
        m_std_cells.connect(inv, invIn);
        m_std_cells.connect(inv, invOut);

        auto ff = m_std_cells.add_cell("DFF_X80");
        auto ffCk = m_std_cells.add_pin("DFF_X80:ck", pin_direction_type::INOUT);
        auto ffIn = m_std_cells.add_pin("DFF_X80:d", pin_direction_type::INPUT);
        auto ffOut = m_std_cells.add_pin("DFF_X80:q", pin_direction_type::OUTPUT);
        m_std_cells.connect(ff, ffCk);
        m_std_cells.connect(ff, ffIn);
        m_std_cells.connect(ff, ffOut);
    }
};
} // namespace

TEST_CASE_METHOD(LibraryFixture, "Library: init", "[timing][library]")
{
    SECTION("Library: info about timing arcs in late mode", "[timing][library]")
    {

        CHECK(m_arcs.size() == 0);
        timing::Library lib(m_std_cells, m_arcs);
        lib.init(*m_liberty.get(), false);

        CHECK(m_arcs.size() == 3);

        CHECK(!lib.cell_sequential(m_std_cells.find_cell("INV_X1")));
        CHECK(!lib.pin_clock(m_std_cells.find_pin("INV_X1:a")));
        CHECK(!lib.pin_clock(m_std_cells.find_pin("INV_X1:o")));

        CHECK(lib.cell_sequential(m_std_cells.find_cell("DFF_X80")));
        CHECK(lib.pin_clock(m_std_cells.find_pin("DFF_X80:ck")));
        CHECK(!lib.pin_clock(m_std_cells.find_pin("DFF_X80:d")));
        CHECK(!lib.pin_clock(m_std_cells.find_pin("DFF_X80:q")));

        int i = 0;
        for (auto arcIt = m_arcs.begin(); arcIt != m_arcs.end(); arcIt++, i++)
        {
            auto arc = *arcIt;
            switch (i) {
            case 0:
                CHECK(m_std_cells.name(m_arcs.from(arc)) == "INV_X1:a");
                CHECK(m_std_cells.name(m_arcs.to(arc)) == "INV_X1:o");
                CHECK(lib.unateness(arc) == unateness_type::NEGATIVE_UNATE);
                CHECK(lib.type(arc) == timing_edge_type::COMBINATIONAL);
                CHECK(diff_E_max(lib.compute_rise_delay(arc, capacitance_unit_type(1.5),  time_unit_type(20.0)) , time_unit_type(1.1565e+14)));
                CHECK(diff_E_max(lib.compute_fall_delay(arc, capacitance_unit_type(0.75), time_unit_type(325.0)), time_unit_type(9.39656e+14)));
                CHECK(diff_E_max(lib.compute_rise_slews(arc, capacitance_unit_type(18.5), time_unit_type(18.5)), time_unit_type(-1.36301e+15)));
                CHECK(diff_E_max(lib.compute_fall_slews(arc, capacitance_unit_type(8.0),  time_unit_type(300.0)), time_unit_type(-9.558e+15)));
                CHECK(diff_E_min(lib.capacitance(m_arcs.from(arc)), capacitance_unit_type(1e-15)));
                CHECK(lib.capacitance(m_arcs.to(arc)) == capacitance_unit_type(0.0));
                break;
            case 1:
                CHECK(m_std_cells.name(m_arcs.from(arc)) == "DFF_X80:ck");
                CHECK(m_std_cells.name(m_arcs.to(arc)) == "DFF_X80:q");
                CHECK(lib.unateness(arc) == unateness_type::NON_UNATE);
                CHECK(lib.type(arc) == timing_edge_type::RISING_EDGE);
                CHECK(lib.compute_rise_delay(arc, capacitance_unit_type(128.0), time_unit_type(30.0)) == time_unit_type(6000));
                CHECK(lib.compute_fall_delay(arc, capacitance_unit_type(2048.0), time_unit_type(300.0)) == time_unit_type(96000));
                CHECK(lib.compute_rise_slews(arc, capacitance_unit_type(512.0), time_unit_type(200.0)) == time_unit_type(24000));
                CHECK(lib.compute_fall_slews(arc, capacitance_unit_type(32.0), time_unit_type(32.0)) == time_unit_type(1500));
                CHECK(diff_E_min(lib.capacitance(m_arcs.from(arc)), capacitance_unit_type(1.5e-15)));
                CHECK(lib.capacitance(m_arcs.to(arc)) == capacitance_unit_type(0.0));
                break;
            case 2:
                CHECK(m_std_cells.name(m_arcs.from(arc)) == "DFF_X80:ck");
                CHECK(m_std_cells.name(m_arcs.to(arc)) == "DFF_X80:d");
                CHECK(lib.type(arc) == timing_edge_type::SETUP_RISING);
                CHECK(diff_E_min(lib.compute_rise_slews(arc, capacitance_unit_type(1), time_unit_type(1)), time_unit_type(1.5e-12)));
                CHECK(diff_E_min(lib.compute_fall_slews(arc, capacitance_unit_type(1), time_unit_type(1)), time_unit_type(2.5e-12)));
                CHECK(diff_E_min(lib.capacitance(m_arcs.from(arc)), capacitance_unit_type(1.5e-15)));
                CHECK(diff_E_min(lib.capacitance(m_arcs.to(arc)), capacitance_unit_type(3.49e-15)));
                break;
            default:
                break;
            }
        }
    }

    SECTION("Library: info about timing arcs in early mode", "[timing][library]")
    {
        CHECK(m_arcs.size() == 0);
        timing::Library lib(m_std_cells, m_arcs);
        lib.init(*m_liberty.get(), true);
        CHECK(m_arcs.size() == 3);

        int i = 0;
        for (auto arcIt = m_arcs.begin(); arcIt != m_arcs.end(); arcIt++, i++)
        {
            auto arc = *arcIt;
            switch (i) {
            case 0:
                CHECK(m_std_cells.name(m_arcs.from(arc)) == "INV_X1:a");
                CHECK(m_std_cells.name(m_arcs.to(arc)) == "INV_X1:o");
                CHECK(lib.unateness(arc) == unateness_type::NEGATIVE_UNATE);
                CHECK(lib.type(arc) == timing_edge_type::COMBINATIONAL);
                CHECK(diff_E_max(lib.compute_rise_delay(arc, capacitance_unit_type(1.5), time_unit_type(20.0)), time_unit_type(1.1565e+14)));
                CHECK(diff_E_max(lib.compute_fall_delay(arc, capacitance_unit_type(0.75), time_unit_type(325.0)), time_unit_type(9.39656e+14)));
                CHECK(diff_E_max(lib.compute_rise_slews(arc, capacitance_unit_type(18.5), time_unit_type(18.5)), time_unit_type(-1.36301e+15)));
                CHECK(diff_E_max(lib.compute_fall_slews(arc, capacitance_unit_type(8.0), time_unit_type(300.0)), time_unit_type(-9.558e+15)));
                break;
            case 1:
                CHECK(m_std_cells.name(m_arcs.from(arc)) == "DFF_X80:ck");
                CHECK(m_std_cells.name(m_arcs.to(arc)) == "DFF_X80:q");
                CHECK(lib.unateness(arc) == unateness_type::NON_UNATE);
                CHECK(lib.type(arc) == timing_edge_type::RISING_EDGE);
                CHECK(lib.compute_rise_delay(arc, capacitance_unit_type(128.0), time_unit_type(30.0)) == time_unit_type(6000));
                CHECK(lib.compute_fall_delay(arc, capacitance_unit_type(2048.0), time_unit_type(300.0)) == time_unit_type(96000));
                CHECK(lib.compute_rise_slews(arc, capacitance_unit_type(512.0), time_unit_type(200.0)) == time_unit_type(24000));
                CHECK(lib.compute_fall_slews(arc, capacitance_unit_type(32.0), time_unit_type(32.0)) == time_unit_type(1500));
                break;
            case 2:
                CHECK(m_std_cells.name(m_arcs.from(arc)) == "DFF_X80:ck");
                CHECK(m_std_cells.name(m_arcs.to(arc)) == "DFF_X80:d");
                CHECK(lib.type(arc) == timing_edge_type::HOLD_RISING);
                CHECK(diff_E_min(lib.compute_rise_slews(arc, capacitance_unit_type(1), time_unit_type(1)), time_unit_type(3.5e-12)));
                CHECK(diff_E_min(lib.compute_fall_slews(arc, capacitance_unit_type(1), time_unit_type(1)), time_unit_type(4.5e-12)));
                break;
            default:
                break;
            }
        }
    }
}



