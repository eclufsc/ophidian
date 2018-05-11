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
#include <ophidian/design/DesignBuilder.h>
#include <ophidian/timing/Library.h>
#include <ophidian/timing/TimingGraphBuilder.h>
#include <ophidian/timing/TimingData.h>
#include <ophidian/parser/SDCParser.h>

using namespace ophidian;

using design_type         = design::Design;
using design_builder_type = design::ICCAD2015ContestDesignBuilder;
using timing_arcs_type    = timing::TimingArcs;
using timing_library_type = timing::Library;
using timing_graph_type   = timing::TimingGraph;
using graph_builder_type  = timing::TimingGraphBuilder;
using node_type           = timing_graph_type::node_type;
using arc_type            = timing_graph_type::arc_type;
using timing_data_type    = timing::TimingData;

namespace
{
class WiremodelFixture
{
public:
    design::ICCAD2015ContestDesignBuilder mBuilder;
    design::Design & mDesign;

    std::shared_ptr<ophidian::parser::Liberty> mLiberty;
    timing::TimingArcs mTimingArcs;
    timing::Library mTimingLibrary;
    std::shared_ptr<parser::DesignConstraints> mDC;

    std::shared_ptr<timing::TimingGraph> mGraph;

    WiremodelFixture() :
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

TEST_CASE_METHOD(WiremodelFixture, "Wiremodel: init", "[timing][wiremodel]")
{
    REQUIRE(mGraph->size(timing_graph_type::node_type()) == 42);
    REQUIRE(mGraph->size(timing_graph_type::arc_type()) == 40);
}

