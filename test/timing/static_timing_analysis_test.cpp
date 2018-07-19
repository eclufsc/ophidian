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

#include <ophidian/timing/StaticTimingAnalysis.h>
#include <ophidian/design/DesignBuilder.h>
#include <ophidian/parser/LibertyParser.h>
#include <ophidian/circuit/LibraryMapping.h>
#include <ophidian/timing/TimingGraphBuilder.h>
#include <ophidian/timing/WireModels.h>
#include <ophidian/timingdriven_placement/FluteRCTreeEstimation.h>

using namespace ophidian;

using slew_type = timing::GenericSTA<timing::wiremodel::LumpedCapacitance, timing::Optimistic>::slew_unit_type;

namespace
{
class STAFixture
{
public:
    design::ICCAD2015ContestDesignBuilder mBuilder;
    design::Design & mDesign;

    std::shared_ptr<parser::Liberty> mEarlyLiberty, mLateLiberty;
    std::shared_ptr<parser::DesignConstraints> mDC;
    std::unique_ptr<parser::Lef> mLef;

    template<class Value>
    bool diff(const Value &a, const Value &b, int exp)
    {
        return units::math::abs(a - b) < Value(std::pow(10, exp-1));
    }

    STAFixture() :
        mBuilder("./input_files/simple/simple.lef",
                 "./input_files/simple/simple.def",
                 "./input_files/simple/simple.v",
                 "./input_files/simple/simple_Early.lib",
                 "./input_files/simple/simple_Late.lib",
                 "./input_files/simple/simple.sdc"),
        mDesign(mBuilder.build()),
        mEarlyLiberty(parser::LibertyParser().readFile("./input_files/simple/simple_Early.lib")),
        mLateLiberty(parser::LibertyParser().readFile("./input_files/simple/simple_Late.lib")),
        mDC(parser::SDCSimple().constraints())
    {
        mLef = std::make_unique<parser::Lef>();
        parser::LefParser lef_parser;
        lef_parser.readFile("input_files/simple/simple.lef", mLef);
    }
};
} // namespace

TEST_CASE_METHOD(STAFixture, "StaticTimingAnalysis: generals tests", "[timing][sta]")
{
    timing::StaticTimingAnalysis sta(mDesign.standardCells(),
                                     mDesign.netlist(),
                                     mDesign.libraryMapping(),
                                     mDesign.placement(),
                                     mDesign.placementMapping());

    sta.init(*mEarlyLiberty, *mLateLiberty, *mLef, *mDC);

    auto pin = mDesign.netlist().find(circuit::Pin(), "inp1");
    CHECK(sta.early_rise_arrival(pin) == slew_type(0));
    CHECK(sta.early_fall_arrival(pin) == slew_type(0));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(1e-11), -11));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(1e-11), -11));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.54282e-11), -11));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(1.11228e-10), -10));

    pin = mDesign.netlist().find(circuit::Pin(), "inp2");
    CHECK(sta.early_rise_arrival(pin) == slew_type(0));
    CHECK(sta.early_fall_arrival(pin) == slew_type(0));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(1e-11), -11));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(1e-11), -11));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.54282e-11), -11));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(1.11228e-10), -10));

    pin = mDesign.netlist().find(circuit::Pin(), "iccad_clk");
    CHECK(sta.early_rise_arrival(pin) == slew_type(0));
    CHECK(sta.early_fall_arrival(pin) == slew_type(0));
    CHECK(sta.early_rise_slew(pin) == slew_type(0));
    CHECK(sta.early_fall_slew(pin) == slew_type(0));
    CHECK(sta.early_rise_slack(pin) == timing::Optimistic::best());
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.16079e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "out");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(5.32484e-11), -11));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(5.32484e-11), -11));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(1.86528e-11), -11));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(1.86528e-11), -11));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.32484e-11), -11));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.32484e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "u1:a");
    CHECK(sta.early_rise_arrival(pin) == slew_type(0));
    CHECK(sta.early_fall_arrival(pin) == slew_type(0));
    CHECK(sta.early_rise_slew(pin) == slew_type(0));
    CHECK(sta.early_fall_slew(pin) == slew_type(0));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(1.11228e-10), -10));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.54282e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "u1:b");
    CHECK(sta.early_rise_arrival(pin) == slew_type(0));
    CHECK(sta.early_fall_arrival(pin) == slew_type(0));
    CHECK(sta.early_rise_slew(pin) == slew_type(0));
    CHECK(sta.early_fall_slew(pin) == slew_type(0));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(1.11228e-10), -10));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.54282e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "u1:o");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(1.82444e-11), -11));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(3.64877e-11), -11));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(2.18811e-11), -11));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(4.3784e-11), -11));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.54282e-11), -11));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(1.11228e-10), -10));

    pin = mDesign.netlist().find(circuit::Pin(), "u2:o");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(8.43065e-11), -11));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(5.15191e-11), -11));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(7.8364e-11), -11));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(3.9182e-11), -11));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(8.43953e-11), -11));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.16079e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "u2:a");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(1.82515e-11), -11));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(3.64949e-11), -11));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(2.18811e-11), -11));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(4.3784e-11), -11));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.54282e-11), -11));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(1.11228e-10), -10));

    pin = mDesign.netlist().find(circuit::Pin(), "u2:b");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(1.62544e-11), -11));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(1.62544e-11), -11));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(1.28806e-11), -11));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(1.28806e-11), -11));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.16079e-11), -11));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(8.43953e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "f1:d");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(8.43953e-11), -11));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(5.16079e-11), -11));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(7.83641e-11), -11));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(3.91821e-11), -11));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(8.43953e-11), -11));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.16079e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "f1:ck");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(3.3589e-12), -12));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(3.3589e-12), -12));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(3.47935e-12), -12));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(3.47935e-12), -12));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.16079e-11), -11));
    CHECK(sta.early_fall_slack(pin) == timing::Optimistic::best());

    pin = mDesign.netlist().find(circuit::Pin(), "f1:q");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(1.62395e-11), -11));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(1.62395e-11), -11));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(1.28806e-11), -11));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(1.28806e-11), -11));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.16079e-11), -11));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.32484e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "u3:a");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(1.62576e-11), -11));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(1.62576e-11), -11));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(1.28806e-11), -11));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(1.28806e-11), -11));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.32484e-11), -11));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.32484e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "u3:o");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(3.39717e-11), -11));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(3.39717e-11), -11));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(1.81002e-11), -11));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(1.81002e-11), -11));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.32484e-11), -11));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.32484e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "u4:a");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(3.39843e-11), -11));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(3.39843e-11), -11));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(1.81003e-11), -11));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(1.81003e-11), -11));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.32484e-11), -11));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.32484e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "u4:o");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(5.32251e-11), -11));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(5.32251e-11), -11));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(1.86528e-11), -11));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(1.86528e-11), -11));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.32484e-11), -11));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.32484e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "lcb1:a");
    CHECK(sta.early_rise_arrival(pin) == slew_type(0));
    CHECK(sta.early_fall_arrival(pin) == slew_type(0));
    CHECK(sta.early_rise_slew(pin) == slew_type(0));
    CHECK(sta.early_fall_slew(pin) == slew_type(0));
    CHECK(sta.early_rise_slack(pin) == timing::Optimistic::best());
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.16079e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "lcb1:o");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(3.33128e-12), -12));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(3.33128e-12), -12));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(3.47924e-12), -12));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(3.47924e-12), -12));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.16079e-11), -11));
    CHECK(sta.early_fall_slack(pin) == timing::Optimistic::best());

    /* ------- Late ---------*/

    pin = mDesign.netlist().find(circuit::Pin(), "inp1");
    CHECK(sta.late_rise_arrival(pin) == slew_type(0));
    CHECK(sta.late_fall_arrival(pin) == slew_type(0));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(1e-11), -11));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(1e-11), -11));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-6.41949e-12), -12));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-9.37997e-11), -11));

    REQUIRE(sta.late_rise_slack(pin) == slew_type(-6.41949e-12));
//    REQUIRE(sta.late_fall_slack(pin) == slew_type(-9.19408e-11));

    pin = mDesign.netlist().find(circuit::Pin(), "inp2");
    CHECK(sta.late_rise_arrival(pin) == slew_type(0));
    CHECK(sta.late_fall_arrival(pin) == slew_type(0));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(1e-11), -11));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(1e-11), -11));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-5.56059e-12), -12));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-9.19408e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "iccad_clk");
    CHECK(sta.late_rise_arrival(pin) == slew_type(0));
    CHECK(sta.late_fall_arrival(pin) == slew_type(0));
    CHECK(sta.late_rise_slew(pin) == slew_type(0));
    CHECK(sta.late_fall_slew(pin) == slew_type(0));
    CHECK(sta.late_rise_slack(pin) == timing::Optimistic::best());
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-8.4687e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "out");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(8.4687e-11), -11));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(8.4687e-11), -11));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(2.80637e-11), -11));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(2.80637e-11), -11));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-8.4687e-11), -11));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-8.4687e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "u1:a");
    CHECK(sta.late_rise_arrival(pin) == slew_type(0));
    CHECK(sta.late_fall_arrival(pin) == slew_type(0));
    CHECK(sta.late_rise_slew(pin) == slew_type(0));
    CHECK(sta.late_fall_slew(pin) == slew_type(0));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-9.19408e-11), -11));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-5.56059e-12), -12));

    pin = mDesign.netlist().find(circuit::Pin(), "u1:b");
    CHECK(sta.late_rise_arrival(pin) == slew_type(0));
    CHECK(sta.late_fall_arrival(pin) == slew_type(0));
    CHECK(sta.late_rise_slew(pin) == slew_type(0));
    CHECK(sta.late_fall_slew(pin) == slew_type(0));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-9.19408e-11), -11));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-5.56059e-12), -12));

    pin = mDesign.netlist().find(circuit::Pin(), "u1:o");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(2.73666e-11), -11));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(5.47316e-11), -11));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(3.28216e-11), -11));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(6.5676e-11), -11));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-5.56059e-12), -12));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-9.19408e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "u2:o");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(1.73711e-10), -10));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(8.63307e-11), -11));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(1.17546e-10), -10));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(5.87877e-11), -11));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-9.19408e-11), -11));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-5.56059e-12), -12));

    pin = mDesign.netlist().find(circuit::Pin(), "u2:a");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(2.73737e-11), -11));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(5.47388e-11), -11));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(3.28216e-11), -11));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(6.5676e-11), -11));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-5.56059e-12), -12));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-9.19408e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "u2:b");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(2.43603e-11), -11));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(2.43603e-11), -11));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(1.9321e-11), -11));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(1.9321e-11), -11));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(1.55581e-12), -12));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-4.67296e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "f1:d");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(1.738e-10), -10));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(8.64195e-11), -11));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(1.17546e-10), -10));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(5.87878e-11), -11));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-9.19408e-11), -11));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-5.56059e-12), -12));

    pin = mDesign.netlist().find(circuit::Pin(), "f1:ck");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(5.02454e-12), -12));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(5.02454e-12), -12));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(5.21894e-12), -12));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(5.21894e-12), -12));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-8.4687e-11), -11));
    CHECK(sta.late_fall_slack(pin) == timing::Optimistic::best());

    pin = mDesign.netlist().find(circuit::Pin(), "f1:q");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(2.43455e-11), -11));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(2.43455e-11), -11));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(1.9321e-11), -11));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(1.9321e-11), -11));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-8.4687e-11), -11));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-8.4687e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "u3:a");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(2.43636e-11), -11));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(2.43636e-11), -11));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(1.9321e-11), -11));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(1.9321e-11), -11));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-8.4687e-11), -11));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-8.4687e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "u3:o");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(5.29441e-11), -11));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(5.29441e-11), -11));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(2.72218e-11), -11));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(2.72218e-11), -11));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-8.4687e-11), -11));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-8.4687e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "u4:a");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(5.29567e-11), -11));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(5.29567e-11), -11));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(2.72218e-11), -11));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(2.72218e-11), -11));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-8.4687e-11), -11));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-8.4687e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "u4:o");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(8.46638e-11), -11));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(8.46638e-11), -11));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(2.80637e-11), -11));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(2.80637e-11), -11));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-8.4687e-11), -11));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-8.4687e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "lcb1:a");
    CHECK(sta.late_rise_arrival(pin) == slew_type(0));
    CHECK(sta.late_fall_arrival(pin) == slew_type(0));
    CHECK(sta.late_rise_slew(pin) == slew_type(0));
    CHECK(sta.late_fall_slew(pin) == slew_type(0));
    CHECK(sta.late_rise_slack(pin) == timing::Optimistic::best());
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-8.4687e-11), -11));

    pin = mDesign.netlist().find(circuit::Pin(), "lcb1:o");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(4.99692e-12), -12));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(4.99692e-12), -12));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(5.21887e-12), -12));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(5.21887e-12), -12));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-8.4687e-11), -11));
    CHECK(sta.late_fall_slack(pin) == timing::Optimistic::best());

    REQUIRE(sta.timing_endpoints().size() == 2);
    CHECK(mDesign.netlist().name(*sta.timing_endpoints().begin()) == "out");
    CHECK(mDesign.netlist().name(*(++sta.timing_endpoints().begin())) == "f1:d");

    /* ---- WNS and TNS ---- */
    CHECK(sta.early_tns() == slew_type(0));
    CHECK(diff(sta.early_wns(), slew_type(4.20834e-11), -11));
    CHECK(diff(sta.late_tns(), slew_type(-1.76628e-10), -10));
    CHECK(diff(sta.late_wns(), slew_type(-9.19408e-11), -11));
}

