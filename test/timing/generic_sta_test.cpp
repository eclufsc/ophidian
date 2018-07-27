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

#include <ophidian/timing/GenericSTA.h>
#include <ophidian/design/DesignBuilder.h>
#include <ophidian/parser/LibertyParser.h>
#include <ophidian/circuit/LibraryMapping.h>
#include <ophidian/timing/TimingGraphBuilder.h>
#include <ophidian/timing/WireModels.h>
#include <ophidian/timingdriven_placement/FluteRCTreeEstimation.h>
#include <cmath>

using namespace ophidian;

using lumped_optimistic_sta_type = timing::GenericSTA<timing::wiremodel::LumpedCapacitance, timing::Optimistic>;
using effective_optimistic_sta_type = timing::GenericSTA<timing::wiremodel::EffectiveCapacitance, timing::Optimistic>;
using slew_type        = lumped_optimistic_sta_type::slew_unit_type;
using slew_type        = lumped_optimistic_sta_type::slew_unit_type;
using capacitance_type = lumped_optimistic_sta_type::capacitance_unit_type;
using pin_type         = lumped_optimistic_sta_type::pin_entity_type;

namespace
{
class GenericSTAFixture
{
public:
    design::ICCAD2015ContestDesignBuilder      m_builder;
    design::Design &                           m_design;

    std::shared_ptr<ophidian::parser::Liberty> m_liberty;
    timing::TimingArcs                         m_timing_arcs;
    timing::Library                            m_timing_library;
    std::shared_ptr<parser::DesignConstraints> m_dc;
    std::unique_ptr<parser::Lef>               m_lef;

    timing::TimingGraph                        m_graph;

    template<class Value>
    bool diff(const Value &a, const Value &b, int exp)
    {
        auto x = units::math::abs(a - b);
        auto y = Value(std::pow(10, exp-1));
        return x < y;
    }

    GenericSTAFixture() :
        m_builder("./input_files/simple/simple.lef",
                 "./input_files/simple/simple.def",
                "./input_files/simple/simple.v",
                 "./input_files/simple/simple_Early.lib",
                 "./input_files/simple/simple_Late.lib",
                 "./input_files/simple/simple.sdc"),
        m_design(m_builder.build()),
        m_liberty(parser::LibertyParser().readFile("./input_files/simple/simple_Early.lib")),
        m_timing_arcs(m_design.standardCells()),
        m_timing_library(m_design.standardCells(), m_timing_arcs),
        m_dc(parser::SDCSimple().constraints()),
        m_graph(m_design.netlist())
    {
        m_timing_library.init(*m_liberty, true);
        timing::TimingGraphBuilder().build(m_design.netlist(),
                                           m_design.standardCells(),
                                           m_design.libraryMapping(),
                                           m_timing_arcs,
                                           m_timing_library,
                                           *m_dc, m_graph);
        m_lef = std::make_unique<parser::Lef>();
        parser::LefParser lef_parser;
        lef_parser.readFile("input_files/simple/simple.lef", m_lef);
    }
};
} // namespace

TEST_CASE("GenericSTA: Estimation", "[timing][sta]")
{
    SECTION("GenericSTA: Optimistic estimation", "[timing][sta][estimation]")
    {
        timing::Optimistic opt;
        using time_unit_type = timing::Optimistic::time_unit_type;

        CHECK(opt(time_unit_type(1), time_unit_type(2)) == time_unit_type(1));
        CHECK(opt.inverted(time_unit_type(1), time_unit_type(2)) == time_unit_type(2));
        CHECK(opt.best() == std::numeric_limits<time_unit_type>::max());
        CHECK(opt.worst() == -std::numeric_limits<time_unit_type>::max());
        CHECK(opt.slack_signal() == -1.0);
    }

    SECTION("GenericSTA: Pessimistic estimation", "[timing][sta][estimation]")
    {
        timing::Pessimistic pes;
        using time_unit_type = timing::Pessimistic::time_unit_type;

        CHECK(pes(time_unit_type(1), time_unit_type(2)) == time_unit_type(2));
        CHECK(pes.inverted(time_unit_type(1), time_unit_type(2)) == time_unit_type(1));
        CHECK(pes.best() == -std::numeric_limits<time_unit_type>::max());
        CHECK(pes.worst() == std::numeric_limits<time_unit_type>::max());
        CHECK(pes.slack_signal() == 1.0);
    }
}


TEST_CASE_METHOD(GenericSTAFixture, "GenericSTA: generals tests", "[timing][sta]")
{
    SECTION("Generic STA: GraphAndTopology", "[timing][sta][topology]")
    {
        timing::GraphAndTopology topology(m_graph, m_design.netlist(), m_design.standardCells(), m_design.libraryMapping());
        topology.init();

        using SortedIndex       = std::size_t;
        using SortedDriverIndex = std::size_t;
        using LevelIndex        = std::size_t;

        SortedIndex       s_u1,  s_u2,  s_u3,  s_u4;
        SortedDriverIndex sd_u1, sd_u2, sd_u3, sd_u4;
        LevelIndex        l_u1,  l_u2,  l_u3,  l_u4;

        for (SortedIndex i(0); i < topology.m_sorted.size(); ++i)
            if (m_design.netlist().name(m_graph.entity(topology.m_sorted[i])) == "u1:o")
                s_u1 = i;
            else if (m_design.netlist().name(m_graph.entity(topology.m_sorted[i])) == "u2:o")
                s_u2 = i;
            else if (m_design.netlist().name(m_graph.entity(topology.m_sorted[i])) == "u3:o")
                s_u3 = i;
            else if (m_design.netlist().name(m_graph.entity(topology.m_sorted[i])) == "u4:o")
                s_u4 = i;

        for (SortedDriverIndex i(0); i < topology.m_sorted_drivers.size(); ++i)
            if (m_design.netlist().name(m_graph.entity(topology.m_sorted_drivers[i])) == "u1:o")
                sd_u1 = i;
            else if (m_design.netlist().name(m_graph.entity(topology.m_sorted_drivers[i])) == "u2:o")
                sd_u2 = i;
            else if (m_design.netlist().name(m_graph.entity(topology.m_sorted_drivers[i])) == "u3:o")
                sd_u3 = i;
            else if (m_design.netlist().name(m_graph.entity(topology.m_sorted_drivers[i])) == "u4:o")
                sd_u4 = i;

        for (LevelIndex i(0); i < topology.m_levels.size(); ++i)
            for (auto n : topology.m_levels[i])
                if (m_design.netlist().name(m_graph.entity(n)) == "u1:o")
                    l_u1 = i;
                else if (m_design.netlist().name(m_graph.entity(n)) == "u2:o")
                    l_u2 = i;
                else if (m_design.netlist().name(m_graph.entity(n)) == "u3:o")
                    l_u3 = i;
                else if (m_design.netlist().name(m_graph.entity(n)) == "u4:o")
                    l_u4 = i;

        CHECK(s_u1 < s_u2);
        CHECK(s_u3 < s_u4);

        CHECK(sd_u1 < sd_u2);
        CHECK(sd_u3 < sd_u4);

        CHECK(l_u1 < l_u2);
        CHECK(l_u3 < l_u4);
    }

    SECTION("Generic STA: Lumped", "[timing][sta]")
    {
        timing::GraphAndTopology topology(m_graph, m_design.netlist(), m_design.standardCells(), m_design.libraryMapping());
        topology.init();
        timing::TimingData data(m_timing_library, m_graph);

        auto rctree_property = m_design.netlist().makeProperty<timingdriven_placement::RCTree>(circuit::Net());
        timingdriven_placement::FluteRCTreeBuilder builder;

        for (auto it = m_design.netlist().begin(circuit::Net()); it != m_design.netlist().end(circuit::Net()); ++it)
        {
            pin_type source;
            const circuit::Net & net = *it;
            timingdriven_placement::RCTree & rctree = rctree_property[net];

            for (auto pin : m_design.netlist().pins(net))
            {
                auto direct = m_design.standardCells().direction(m_design.libraryMapping().pinStdCell(pin));
                if (direct == standard_cell::PinDirection::OUTPUT)
                {
                    source = pin;
                    break;
                }
            }

            builder.build(m_design.placement(), m_design.placementMapping(), m_design.libraryMapping(), m_design.netlist(), m_timing_library, *m_lef, net, rctree, source);
        }

        lumped_optimistic_sta_type sta(data, topology, rctree_property);
        sta.init(*m_dc);

        sta.update_ats();
        sta.update_rts();

        auto pin = m_design.netlist().find(pin_type(), "inp1");
        CHECK(sta.rise_arrival(pin) == slew_type(0));
        CHECK(sta.fall_arrival(pin) == slew_type(0));
        CHECK(diff(sta.rise_slew(pin), slew_type(1e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(1e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.52792e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(1.10928e-10), -13));

        pin = m_design.netlist().find(pin_type(), "inp2");
        CHECK(sta.rise_arrival(pin) == slew_type(0));
        CHECK(sta.fall_arrival(pin) == slew_type(0));
        CHECK(diff(sta.rise_slew(pin), slew_type(1e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(1e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.52792e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(1.10928e-10), -13));

        pin = m_design.netlist().find(pin_type(), "iccad_clk");
        CHECK(sta.rise_arrival(pin) == slew_type(0));
        CHECK(sta.fall_arrival(pin) == slew_type(0));
        CHECK(sta.rise_slew(pin) == slew_type(0));
        CHECK(sta.fall_slew(pin) == slew_type(0));
        CHECK(sta.rise_slack(pin) == timing::Optimistic::best());
        CHECK(diff(sta.fall_slack(pin), slew_type(5.16088e-11), -13));

        pin = m_design.netlist().find(pin_type(), "out");
        CHECK(diff(sta.rise_arrival(pin), slew_type(5.33238e-11), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(5.33238e-11), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(1.82773e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(1.82773e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.33238e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(5.33238e-11), -13));

        pin = m_design.netlist().find(pin_type(), "u1:a");
        CHECK(sta.rise_arrival(pin) == slew_type(0));
        CHECK(sta.fall_arrival(pin) == slew_type(0));
        CHECK(sta.rise_slew(pin) == slew_type(0));
        CHECK(sta.fall_slew(pin) == slew_type(0));
        CHECK(diff(sta.rise_slack(pin), slew_type(1.10928e-10), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(5.52792e-11), -13));

        pin = m_design.netlist().find(pin_type(), "u1:b");
        CHECK(sta.rise_arrival(pin) == slew_type(0));
        CHECK(sta.fall_arrival(pin) == slew_type(0));
        CHECK(sta.rise_slew(pin) == slew_type(0));
        CHECK(sta.fall_slew(pin) == slew_type(0));
        CHECK(diff(sta.rise_slack(pin), slew_type(1.10928e-10), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(5.52792e-11), -13));

        pin = m_design.netlist().find(pin_type(), "u1:o");
        CHECK(diff(sta.rise_arrival(pin), slew_type(1.8121e-11), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(3.6241e-11), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(2.17325e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(4.3488e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.52792e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(1.10928e-10), -13));

        pin = m_design.netlist().find(pin_type(), "u2:o");
        CHECK(diff(sta.rise_arrival(pin), slew_type(8.43123e-11), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(5.15199e-11), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(7.8376e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(3.9188e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(8.44012e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(5.16088e-11), -13));

        pin = m_design.netlist().find(pin_type(), "u2:a");
        CHECK(diff(sta.rise_arrival(pin), slew_type(1.81275e-11), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(3.62474e-11), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(2.17325e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(4.3488e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.52792e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(1.10928e-10), -13));

        pin = m_design.netlist().find(pin_type(), "u2:b");
        CHECK(diff(sta.rise_arrival(pin), slew_type(1.62505e-11), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(1.62505e-11), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(1.28791e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(1.28791e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.16088e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(8.44012e-11), -13));

        pin = m_design.netlist().find(pin_type(), "f1:d");
        CHECK(diff(sta.rise_arrival(pin), slew_type(8.44012e-11), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(5.16088e-11), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(7.83761e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(3.91881e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(8.44012e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(5.16088e-11), -13));

        pin = m_design.netlist().find(pin_type(), "f1:ck");
        CHECK(diff(sta.rise_arrival(pin), slew_type(3.35856e-12), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(3.35856e-12), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(3.47918e-12), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(3.47918e-12), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.16088e-11), -13));
        CHECK(sta.fall_slack(pin) == timing::Optimistic::best());

        pin = m_design.netlist().find(pin_type(), "f1:q");
        CHECK(diff(sta.rise_arrival(pin), slew_type(1.62377e-11), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(1.62377e-11), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(1.28791e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(1.28791e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.16088e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(5.33238e-11), -13));

        pin = m_design.netlist().find(pin_type(), "u3:a");
        CHECK(diff(sta.rise_arrival(pin), slew_type(1.62538e-11), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(1.62538e-11), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(1.28791e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(1.28791e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.33238e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(5.33238e-11), -13));

        pin = m_design.netlist().find(pin_type(), "u3:o");
        CHECK(diff(sta.rise_arrival(pin), slew_type(3.4292e-11), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(3.4292e-11), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(1.84832e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(1.84832e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.33238e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(5.33238e-11), -13));

        pin = m_design.netlist().find(pin_type(), "u4:a");
        CHECK(diff(sta.rise_arrival(pin), slew_type(3.43066e-11), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(3.43066e-11), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(1.84832e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(1.84832e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.32484e-11), -12));
        CHECK(diff(sta.fall_slack(pin), slew_type(5.32484e-11), -12));

        pin = m_design.netlist().find(pin_type(), "u4:o");
        CHECK(diff(sta.rise_arrival(pin), slew_type(5.33026e-11), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(5.33026e-11), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(1.82773e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(1.82773e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.33238e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(5.33238e-11), -13));

        pin = m_design.netlist().find(pin_type(), "lcb1:a");
        CHECK(sta.rise_arrival(pin) == slew_type(0));
        CHECK(sta.fall_arrival(pin) == slew_type(0));
        CHECK(sta.rise_slew(pin) == slew_type(0));
        CHECK(sta.fall_slew(pin) == slew_type(0));
        CHECK(sta.rise_slack(pin) == timing::Optimistic::best());
        CHECK(diff(sta.fall_slack(pin), slew_type(5.16088e-11), -13));

        pin = m_design.netlist().find(pin_type(), "lcb1:o");
        CHECK(diff(sta.rise_arrival(pin), slew_type(3.33115e-12), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(3.33115e-12), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(3.47907e-12), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(3.47907e-12), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.16088e-11), -13));
        CHECK(sta.fall_slack(pin) == timing::Optimistic::best());
    }

    SECTION("Generic STA: Effective", "[timing][sta]")
    {
        timing::GraphAndTopology topology(m_graph, m_design.netlist(), m_design.standardCells(), m_design.libraryMapping());
        topology.init();
        timing::TimingData data(m_timing_library, m_graph);

        auto rctree_property = m_design.netlist().makeProperty<timingdriven_placement::RCTree>(circuit::Net());
        timingdriven_placement::FluteRCTreeBuilder builder;

        for (auto it = m_design.netlist().begin(circuit::Net()); it != m_design.netlist().end(circuit::Net()); ++it)
        {
            pin_type source;
            const circuit::Net & net = *it;
            timingdriven_placement::RCTree & rctree = rctree_property[net];

            for (auto pin : m_design.netlist().pins(net))
            {
                auto direct = m_design.standardCells().direction(m_design.libraryMapping().pinStdCell(pin));
                if (direct == standard_cell::PinDirection::OUTPUT)
                {
                    source = pin;
                    break;
                }
            }

            builder.build(m_design.placement(), m_design.placementMapping(), m_design.libraryMapping(), m_design.netlist(), m_timing_library, *m_lef, net, rctree, source);
        }

        effective_optimistic_sta_type sta(data, topology, rctree_property);
        sta.init(*m_dc);

        sta.update_ats();
        sta.update_rts();

        auto pin = m_design.netlist().find(pin_type(), "inp1");
        CHECK(sta.rise_arrival(pin) == slew_type(0));
        CHECK(sta.fall_arrival(pin) == slew_type(0));
        CHECK(diff(sta.rise_slew(pin), slew_type(1e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(1e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.5198e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(1.10847e-10), -13));

        pin = m_design.netlist().find(pin_type(), "inp2");
        CHECK(sta.rise_arrival(pin) == slew_type(0));
        CHECK(sta.fall_arrival(pin) == slew_type(0));
        CHECK(diff(sta.rise_slew(pin), slew_type(1e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(1e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.5198e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(1.10847e-10), -13));

        pin = m_design.netlist().find(pin_type(), "iccad_clk");
        CHECK(sta.rise_arrival(pin) == slew_type(0));
        CHECK(sta.fall_arrival(pin) == slew_type(0));
        CHECK(sta.rise_slew(pin) == slew_type(0));
        CHECK(sta.fall_slew(pin) == slew_type(0));
        CHECK(sta.rise_slack(pin) == timing::Optimistic::best());
        CHECK(diff(sta.fall_slack(pin), slew_type(5.15316e-11), -13));

        pin = m_design.netlist().find(pin_type(), "out");
        CHECK(diff(sta.rise_arrival(pin), slew_type(5.33103e-11), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(5.33103e-11), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(1.82894e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(1.82894e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.33103e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(5.33103e-11), -13));

        pin = m_design.netlist().find(pin_type(), "u1:a");
        CHECK(sta.rise_arrival(pin) == slew_type(0));
        CHECK(sta.fall_arrival(pin) == slew_type(0));
        CHECK(sta.rise_slew(pin) == slew_type(0));
        CHECK(sta.fall_slew(pin) == slew_type(0));
        CHECK(diff(sta.rise_slack(pin), slew_type(1.10847e-10), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(5.5198e-11), -13));

        pin = m_design.netlist().find(pin_type(), "u1:b");
        CHECK(sta.rise_arrival(pin) == slew_type(0));
        CHECK(sta.fall_arrival(pin) == slew_type(0));
        CHECK(sta.rise_slew(pin) == slew_type(0));
        CHECK(sta.fall_slew(pin) == slew_type(0));
        CHECK(diff(sta.rise_slack(pin), slew_type(1.10847e-10), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(5.5198e-11), -13));

        pin = m_design.netlist().find(pin_type(), "u1:o");
        CHECK(diff(sta.rise_arrival(pin), slew_type(1.81181e-11), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(3.6238e-11), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(2.1729e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(4.34845e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.5198e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(1.10847e-10), -13));

        pin = m_design.netlist().find(pin_type(), "u2:o");
        CHECK(diff(sta.rise_arrival(pin), slew_type(8.42353e-11), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(5.14427e-11), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(7.82814e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(3.90933e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(8.43242e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(5.15316e-11), -13));

        pin = m_design.netlist().find(pin_type(), "u2:a");
        CHECK(diff(sta.rise_arrival(pin), slew_type(1.81246e-11), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(3.62445e-11), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(2.17355e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(4.3491e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.5198e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(1.10847e-10), -13));

        pin = m_design.netlist().find(pin_type(), "u2:b");
        CHECK(diff(sta.rise_arrival(pin), slew_type(1.62496e-11), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(1.62496e-11), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(1.28918e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(1.28918e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.15316e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(8.43242e-11), -13));

        pin = m_design.netlist().find(pin_type(), "f1:d");
        CHECK(diff(sta.rise_arrival(pin), slew_type(8.43242e-11), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(5.15316e-11), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(7.83704e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(3.91824e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(8.43242e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(5.15316e-11), -13));

        pin = m_design.netlist().find(pin_type(), "f1:ck");
        CHECK(diff(sta.rise_arrival(pin), slew_type(3.35784e-12), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(3.35784e-12), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(3.50577e-12), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(3.50577e-12), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.15316e-11), -13));
        CHECK(sta.fall_slack(pin) == timing::Optimistic::best());

        pin = m_design.netlist().find(pin_type(), "f1:q");
        CHECK(diff(sta.rise_arrival(pin), slew_type(1.62368e-11), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(1.62368e-11), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(1.28789e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(1.28789e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.15316e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(5.33103e-11), -13));

        pin = m_design.netlist().find(pin_type(), "u3:a");
        CHECK(diff(sta.rise_arrival(pin), slew_type(1.62529e-11), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(1.62529e-11), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(1.2895e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(1.2895e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.33103e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(5.33103e-11), -13));

        pin = m_design.netlist().find(pin_type(), "u3:o");
        CHECK(diff(sta.rise_arrival(pin), slew_type(3.42856e-11), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(3.42856e-11), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(1.84728e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(1.84728e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.33103e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(5.33103e-11), -13));

        pin = m_design.netlist().find(pin_type(), "u4:a");
        CHECK(diff(sta.rise_arrival(pin), slew_type(3.43002e-11), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(3.43002e-11), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(1.84874e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(1.84874e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.33103e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(5.33103e-11), -13));

        pin = m_design.netlist().find(pin_type(), "u4:o");
        CHECK(diff(sta.rise_arrival(pin), slew_type(5.32891e-11), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(5.32891e-11), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(1.82681e-11), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(1.82681e-11), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.33103e-11), -13));
        CHECK(diff(sta.fall_slack(pin), slew_type(5.33103e-11), -13));

        pin = m_design.netlist().find(pin_type(), "lcb1:a");
        CHECK(sta.rise_arrival(pin) == slew_type(0));
        CHECK(sta.fall_arrival(pin) == slew_type(0));
        CHECK(sta.rise_slew(pin) == slew_type(0));
        CHECK(sta.fall_slew(pin) == slew_type(0));
        CHECK(sta.rise_slack(pin) == timing::Optimistic::best());
        CHECK(diff(sta.fall_slack(pin), slew_type(5.15316e-11), -13));

        pin = m_design.netlist().find(pin_type(), "lcb1:o");
        CHECK(diff(sta.rise_arrival(pin), slew_type(3.33043e-12), -13));
        CHECK(diff(sta.fall_arrival(pin), slew_type(3.33043e-12), -13));
        CHECK(diff(sta.rise_slew(pin), slew_type(3.47814e-12), -13));
        CHECK(diff(sta.fall_slew(pin), slew_type(3.47814e-12), -13));
        CHECK(diff(sta.rise_slack(pin), slew_type(5.15316e-11), -13));
        CHECK(sta.fall_slack(pin) == timing::Optimistic::best());
    }
}

