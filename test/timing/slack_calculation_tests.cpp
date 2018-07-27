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
#include <ophidian/parser/LibertyParser.h>
#include <ophidian/circuit/LibraryMapping.h>
#include <ophidian/timing/SlackCalculation.h>

using namespace ophidian;

using pin_type = circuit::Pin;

namespace
{
class SlackCalculationFixture
{
public:
    design::ICCAD2015ContestDesignBuilder m_builder;
    design::Design & m_design;

    std::shared_ptr<parser::Liberty> m_liberty;
    timing::TimingArcs m_timing_arcs;
    timing::Library m_timing_library;

    SlackCalculationFixture() :
        m_builder("./input_files/simple/simple.lef",
                  "./input_files/simple/simple.def",
                  "./input_files/simple/simple.v",
                  "./input_files/simple/simple_Early.lib",
                  "./input_files/simple/simple_Late.lib",
                  "./input_files/simple/simple.sdc"),
        m_design(m_builder.build()),
        m_liberty(parser::LibertyParser().readFile("./input_files/simple/simple_Early.lib")),
        m_timing_arcs(m_design.standardCells()),
        m_timing_library(m_design.standardCells(), m_timing_arcs)
    {
        m_timing_library.init(*m_liberty, true);
    }
};
} // namespace

TEST_CASE_METHOD(SlackCalculationFixture, "EndPoints: init", "[timing][endpoints]")
{
    timing::EndPoints endpoints(m_design.netlist(), m_design.libraryMapping(), m_design.standardCells(), m_timing_library);
    endpoints.init();

    auto out  = m_design.netlist().find(pin_type(), "out");
    auto f1_d = m_design.netlist().find(pin_type(), "f1:d");

    CHECK(endpoints.size() == 2);

    for (auto it = endpoints.begin(); it != endpoints.end(); ++it)
        CHECK(((out == *it) || (f1_d == *it)));
}
