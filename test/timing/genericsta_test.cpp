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

    std::shared_ptr<timing::TimingGraph> mGraph;

    GenericSTAFixture() :
        mBuilder("./input_files/simple/simple.lef", "./input_files/simple/simple.def", "./input_files/simple/simple.v"),
        mDesign(mBuilder.build()),
        mLiberty(ophidian::parser::LibertyParser().readFile("./input_files/simple/simple_Early.lib")),
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

    }
};
} // namespace

TEST_CASE("TimingGraph: Optimistic estimation", "[timing][sta]")
{
    timing::Optimistic opt;

    REQUIRE(opt(util::second_t(1), util::second_t(2)) == util::second_t(1));
    REQUIRE(opt.inverted(util::second_t(1), util::second_t(2)) == util::second_t(2));
    REQUIRE(opt.best() == std::numeric_limits<util::second_t>::max());
    REQUIRE(opt.worst() == -std::numeric_limits<util::second_t>::max());
    REQUIRE(opt.slackSignal() == -1.0);
}

TEST_CASE("TimingGraph: Pessimistic estimation", "[timing][sta]")
{
    timing::Pessimistic pes;

    REQUIRE(pes(util::second_t(1), util::second_t(2)) == util::second_t(2));
    REQUIRE(pes.inverted(util::second_t(1), util::second_t(2)) == util::second_t(1));
    REQUIRE(pes.best() == -std::numeric_limits<util::second_t>::max());
    REQUIRE(pes.worst() == std::numeric_limits<util::second_t>::max());
    REQUIRE(pes.slackSignal() == 1.0);
}

TEST_CASE_METHOD(GenericSTAFixture, "TimingGraph: GraphAndTopology", "[timing][sta]")
{
    timing::GraphAndTopology topology(*mGraph, mDesign.netlist(), mDesign.standardCells(), mDesign.libraryMapping());

    using SortedIndex       = std::size_t;
    using SortedDriverIndex = std::size_t;
    using LevelIndex        = std::size_t;

    SortedIndex       s_u1,  s_u2,  s_u3,  s_u4;
    SortedDriverIndex sd_u1, sd_u2, sd_u3, sd_u4;
    LevelIndex        l_u1,  l_u2,  l_u3,  l_u4;

    s_u1 = s_u2 = s_u3 = s_u4 = sd_u1 = sd_u2 = sd_u3 = sd_u4 = l_u1 = l_u2 = l_u3 = l_u4 = 0;

    for (SortedIndex i(0); i < topology.mSorted.size(); ++i)
        if (mDesign.netlist().name(mGraph.get()->entity(topology.mSorted[i])) == "u1:o")
            s_u1 = s_u1 > i? s_u1 : i;
        else if (mDesign.netlist().name(mGraph.get()->entity(topology.mSorted[i])) == "u2:o")
            s_u2 = s_u2 > i? s_u2 : i;
        else if (mDesign.netlist().name(mGraph.get()->entity(topology.mSorted[i])) == "u3:o")
            s_u3 = s_u3 > i? s_u3 : i;
        else if (mDesign.netlist().name(mGraph.get()->entity(topology.mSorted[i])) == "u4:o")
            s_u4 = s_u4 > i? s_u4 : i;

    for (SortedDriverIndex i(0); i < topology.mSortedDrivers.size(); ++i)
        if (mDesign.netlist().name(mGraph.get()->entity(topology.mSortedDrivers[i])) == "u1:a")
            sd_u1 = sd_u1 > i? sd_u1 : i;
        else if (mDesign.netlist().name(mGraph.get()->entity(topology.mSortedDrivers[i])) == "u2:a")
            sd_u2 = sd_u2 > i? sd_u2 : i;
        else if (mDesign.netlist().name(mGraph.get()->entity(topology.mSortedDrivers[i])) == "u3:a")
            sd_u3 = sd_u3 > i? sd_u3 : i;
        else if (mDesign.netlist().name(mGraph.get()->entity(topology.mSortedDrivers[i])) == "u4:a")
            sd_u4 = sd_u4 > i? sd_u4 : i;

    for (LevelIndex i(0); i < topology.mLevels.size(); ++i)
        for (auto n : topology.mLevels[i])
            if (mDesign.netlist().name(mGraph.get()->entity(n)) == "u1:o")
                l_u1 = l_u1 > i? l_u1 : i;
            else if (mDesign.netlist().name(mGraph.get()->entity(n)) == "u2:o")
                l_u2 = l_u2 > i? l_u2 : i;
            else if (mDesign.netlist().name(mGraph.get()->entity(n)) == "u3:o")
                l_u3 = l_u3 > i? l_u3 : i;
            else if (mDesign.netlist().name(mGraph.get()->entity(n)) == "u4:o")
                l_u4 = l_u4 > i? l_u4 : i;

    REQUIRE(s_u1 <  s_u2);
    REQUIRE(s_u3 <  s_u4);

    REQUIRE(sd_u1 < sd_u2);
    REQUIRE(sd_u3 < sd_u4);

    REQUIRE(l_u1 <  l_u2);
    REQUIRE(l_u3 <  l_u4);
}

TEST_CASE_METHOD(GenericSTAFixture, "TimingGraph: Generic STA", "[timing][sta]")
{
    timing::GraphAndTopology topology(*mGraph, mDesign.netlist(), mDesign.standardCells(), mDesign.libraryMapping());
}
