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

#include <ophidian/timing/GenericSTA.h>
#include <ophidian/design/DesignBuilder.h>
#include <ophidian/parser/LibertyParser.h>
#include <ophidian/circuit/LibraryMapping.h>
#include <ophidian/timing/TimingGraphBuilder.h>
#include <ophidian/timing/WireModels.h>
#include <ophidian/timingdriven_placement/FluteRCTreeEstimation.h>

using namespace ophidian;

namespace
{
class GenericSTAFixture
{
public:
    design::ICCAD2015ContestDesignBuilder mBuilder;
    design::Design & mDesign;

    std::shared_ptr<ophidian::parser::Liberty> mLiberty;
    timing::TimingArcs mTimingArcs;
    timing::Library mTimingLibrary;
    std::shared_ptr<parser::DesignConstraints> mDC;
    std::unique_ptr<parser::Lef> mLef;

    std::shared_ptr<timing::TimingGraph> mGraph;

    GenericSTAFixture() :
        mBuilder("./input_files/simple/simple.lef", "./input_files/simple/simple.def", "./input_files/simple/simple.v"),
        mDesign(mBuilder.build()),
        mLiberty(parser::LibertyParser().readFile("./input_files/simple/simple_Early.lib")),
        mTimingArcs(mDesign.standardCells()),
        mTimingLibrary(*mLiberty, mDesign.standardCells(), mTimingArcs, true),
        mDC(parser::SDCSimple().constraints()),
        mGraph(timing::TimingGraphBuilder().build(mDesign.netlist(),
                                                  mDesign.standardCells(),
                                                  mDesign.libraryMapping(),
                                                  mTimingArcs,
                                                  mTimingLibrary,
                                                  *mDC))
    {
        mLef = std::make_unique<parser::Lef>();
        parser::LefParser lef_parser;
        lef_parser.readFile("input_files/simple/simple.lef", mLef);
    }
};
} // namespace

TEST_CASE("GenericSTA: Estimation", "[timing][sta]")
{
    SECTION("GenericSTA: Optimistic estimation", "[timing][sta][estimation]")
    {
        timing::Optimistic opt;
        using time_unit_type = timing::Optimistic::time_unit_type;

        REQUIRE(opt(time_unit_type(1), time_unit_type(2)) == time_unit_type(1));
        REQUIRE(opt.inverted(time_unit_type(1), time_unit_type(2)) == time_unit_type(2));
        REQUIRE(opt.best() == std::numeric_limits<time_unit_type>::max());
        REQUIRE(opt.worst() == -std::numeric_limits<time_unit_type>::max());
        REQUIRE(opt.slack_signal() == -1.0);
    }

    SECTION("GenericSTA: Pessimistic estimation", "[timing][sta][estimation]")
    {
        timing::Pessimistic pes;
        using time_unit_type = timing::Pessimistic::time_unit_type;

        REQUIRE(pes(time_unit_type(1), time_unit_type(2)) == time_unit_type(2));
        REQUIRE(pes.inverted(time_unit_type(1), time_unit_type(2)) == time_unit_type(1));
        REQUIRE(pes.best() == -std::numeric_limits<time_unit_type>::max());
        REQUIRE(pes.worst() == std::numeric_limits<time_unit_type>::max());
        REQUIRE(pes.slack_signal() == 1.0);
    }
}


TEST_CASE_METHOD(GenericSTAFixture, "GenericSTA: generals tests", "[timing][sta]")
{
    SECTION("Generic STA: GraphAndTopology", "[timing][sta][topology]")
    {
        timing::GraphAndTopology topology(*mGraph, mDesign.netlist(), mDesign.standardCells(), mDesign.libraryMapping());

        using SortedIndex       = std::size_t;
        using SortedDriverIndex = std::size_t;
        using LevelIndex        = std::size_t;

        SortedIndex       s_u1,  s_u2,  s_u3,  s_u4;
        SortedDriverIndex sd_u1, sd_u2, sd_u3, sd_u4;
        LevelIndex        l_u1,  l_u2,  l_u3,  l_u4;

        s_u1 = s_u2 = s_u3 = s_u4 = sd_u1 = sd_u2 = sd_u3 = sd_u4 = l_u1 = l_u2 = l_u3 = l_u4 = 0;

        for (SortedIndex i(0); i < topology.m_sorted.size(); ++i)
            if (mDesign.netlist().name(mGraph.get()->entity(topology.m_sorted[i])) == "u1:o")
                s_u1 = s_u1 > i? s_u1 : i;
            else if (mDesign.netlist().name(mGraph.get()->entity(topology.m_sorted[i])) == "u2:o")
                s_u2 = s_u2 > i? s_u2 : i;
            else if (mDesign.netlist().name(mGraph.get()->entity(topology.m_sorted[i])) == "u3:o")
                s_u3 = s_u3 > i? s_u3 : i;
            else if (mDesign.netlist().name(mGraph.get()->entity(topology.m_sorted[i])) == "u4:o")
                s_u4 = s_u4 > i? s_u4 : i;

        for (SortedDriverIndex i(0); i < topology.m_sorted_drivers.size(); ++i)
            if (mDesign.netlist().name(mGraph.get()->entity(topology.m_sorted_drivers[i])) == "u1:a")
                sd_u1 = sd_u1 > i? sd_u1 : i;
            else if (mDesign.netlist().name(mGraph.get()->entity(topology.m_sorted_drivers[i])) == "u2:a")
                sd_u2 = sd_u2 > i? sd_u2 : i;
            else if (mDesign.netlist().name(mGraph.get()->entity(topology.m_sorted_drivers[i])) == "u3:a")
                sd_u3 = sd_u3 > i? sd_u3 : i;
            else if (mDesign.netlist().name(mGraph.get()->entity(topology.m_sorted_drivers[i])) == "u4:a")
                sd_u4 = sd_u4 > i? sd_u4 : i;

        for (LevelIndex i(0); i < topology.m_levels.size(); ++i)
            for (auto n : topology.m_levels[i])
                if (mDesign.netlist().name(mGraph.get()->entity(n)) == "u1:o")
                    l_u1 = l_u1 > i? l_u1 : i;
                else if (mDesign.netlist().name(mGraph.get()->entity(n)) == "u2:o")
                    l_u2 = l_u2 > i? l_u2 : i;
                else if (mDesign.netlist().name(mGraph.get()->entity(n)) == "u3:o")
                    l_u3 = l_u3 > i? l_u3 : i;
                else if (mDesign.netlist().name(mGraph.get()->entity(n)) == "u4:o")
                    l_u4 = l_u4 > i? l_u4 : i;

        REQUIRE(s_u1 < s_u2);
        REQUIRE(s_u3 < s_u4);

        REQUIRE(sd_u1 < sd_u2);
        REQUIRE(sd_u3 < sd_u4);

        REQUIRE(l_u1 < l_u2);
        REQUIRE(l_u3 < l_u4);
    }

    SECTION("Generic STA: Init", "[timing][sta]")
    {
        timing::GraphAndTopology topology(*mGraph, mDesign.netlist(), mDesign.standardCells(), mDesign.libraryMapping());
        timing::TimingData data(mTimingLibrary, *mGraph);

        auto rctree_property = mDesign.netlist().makeProperty<timingdriven_placement::RCTree>(circuit::Net());
        timingdriven_placement::FluteRCTreeBuilder builder;

        for (auto it = mDesign.netlist().begin(circuit::Net()); it != mDesign.netlist().end(circuit::Net()); ++it)
        {
            circuit::Pin source;
            const circuit::Net & net = *it;
            timingdriven_placement::RCTree & rctree = rctree_property[net];

            for (auto pin : mDesign.netlist().pins(net))
            {
                auto pin_name = mDesign.netlist().name(pin);
                auto direct = mDesign.standardCells().direction(mDesign.libraryMapping().pinStdCell(pin));
                if (direct == standard_cell::PinDirection::OUTPUT)
                {
                    source = pin;
                    break;
                }
            }

            builder.build(mDesign.placementMapping(), mDesign.libraryMapping(), mDesign.netlist(), mTimingLibrary, *mLef, net, rctree, source);
        }

        timing::GenericSTA<timing::wiremodel::LumpedCapacitance, timing::Optimistic> sta(data, topology, rctree_property);
    }
}

