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
        return units::math::abs(a - b) < Value(std::pow(10.0, exp));
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
    CHECK(diff(sta.early_rise_slew(pin), slew_type(1e-11), -13));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(1e-11), -13));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.52792e-11), -13));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(1.10928e-10), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "inp2");
    CHECK(sta.early_rise_arrival(pin) == slew_type(0));
    CHECK(sta.early_fall_arrival(pin) == slew_type(0));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(1e-11), -13));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(1e-11), -13));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.52792e-11), -13));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(1.10928e-10), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "iccad_clk");
    CHECK(sta.early_rise_arrival(pin) == slew_type(0));
    CHECK(sta.early_fall_arrival(pin) == slew_type(0));
    CHECK(sta.early_rise_slew(pin) == slew_type(0));
    CHECK(sta.early_fall_slew(pin) == slew_type(0));
    CHECK(sta.early_rise_slack(pin) == timing::Optimistic::best());
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.16088e-11), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "out");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(5.33238e-11), -13));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(5.33238e-11), -13));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(1.82773e-11), -13));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(1.82773e-11), -13));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.33238e-11), -13));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.33238e-11), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "u1:a");
    CHECK(sta.early_rise_arrival(pin) == slew_type(0));
    CHECK(sta.early_fall_arrival(pin) == slew_type(0));
    CHECK(sta.early_rise_slew(pin) == slew_type(0));
    CHECK(sta.early_fall_slew(pin) == slew_type(0));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(1.10928e-10), -13));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.52792e-11), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "u1:b");
    CHECK(sta.early_rise_arrival(pin) == slew_type(0));
    CHECK(sta.early_fall_arrival(pin) == slew_type(0));
    CHECK(sta.early_rise_slew(pin) == slew_type(0));
    CHECK(sta.early_fall_slew(pin) == slew_type(0));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(1.10928e-10), -13));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.52792e-11), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "u1:o");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(1.8121e-11), -13));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(3.6241e-11), -13));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(2.17325e-11), -13));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(4.3488e-11), -13));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.52792e-11), -13));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(1.10928e-10), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "u2:o");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(8.43123e-11), -13));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(5.15199e-11), -13));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(7.8376e-11), -13));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(3.9188e-11), -13));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(8.44012e-11), -13));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.16088e-11), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "u2:a");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(1.81275e-11), -13));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(3.62474e-11), -13));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(2.17325e-11), -13));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(4.3488e-11), -13));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.52792e-11), -13));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(1.10928e-10), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "u2:b");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(1.62505e-11), -13));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(1.62505e-11), -13));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(1.28791e-11), -13));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(1.28791e-11), -13));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.16088e-11), -13));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(8.44012e-11), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "f1:d");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(8.44012e-11), -13));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(5.16088e-11), -13));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(7.83761e-11), -13));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(3.91881e-11), -13));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(8.44012e-11), -13));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.16088e-11), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "f1:ck");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(3.35856e-12), -13));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(3.35856e-12), -13));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(3.47918e-12), -13));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(3.47918e-12), -13));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.16088e-11), -13));
    CHECK(sta.early_fall_slack(pin) == timing::Optimistic::best());

    pin = mDesign.netlist().find(circuit::Pin(), "f1:q");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(1.62377e-11), -13));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(1.62377e-11), -13));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(1.28791e-11), -13));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(1.28791e-11), -13));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.16088e-11), -13));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.33238e-11), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "u3:a");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(1.62538e-11), -13));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(1.62538e-11), -13));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(1.28791e-11), -13));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(1.28791e-11), -13));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.33238e-11), -13));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.33238e-11), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "u3:o");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(3.4292e-11), -13));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(3.4292e-11), -13));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(1.84832e-11), -13));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(1.84832e-11), -13));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.33238e-11), -13));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.33238e-11), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "u4:a");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(3.43066e-11), -13));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(3.43066e-11), -13));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(1.84832e-11), -13));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(1.84832e-11), -13));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.32484e-11), -13));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.32484e-11), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "u4:o");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(5.33026e-11), -13));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(5.33026e-11), -13));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(1.82773e-11), -13));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(1.82773e-11), -13));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.33238e-11), -13));
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.33238e-11), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "lcb1:a");
    CHECK(sta.early_rise_arrival(pin) == slew_type(0));
    CHECK(sta.early_fall_arrival(pin) == slew_type(0));
    CHECK(sta.early_rise_slew(pin) == slew_type(0));
    CHECK(sta.early_fall_slew(pin) == slew_type(0));
    CHECK(sta.early_rise_slack(pin) == timing::Optimistic::best());
    CHECK(diff(sta.early_fall_slack(pin), slew_type(5.16088e-11), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "lcb1:o");
    CHECK(diff(sta.early_rise_arrival(pin), slew_type(3.33115e-12), -13));
    CHECK(diff(sta.early_fall_arrival(pin), slew_type(3.33115e-12), -13));
    CHECK(diff(sta.early_rise_slew(pin), slew_type(3.47907e-12), -13));
    CHECK(diff(sta.early_fall_slew(pin), slew_type(3.47907e-12), -13));
    CHECK(diff(sta.early_rise_slack(pin), slew_type(5.16088e-11), -13));
    CHECK(sta.early_fall_slack(pin) == timing::Optimistic::best());

    /* ------- Late ---------*/

    pin = mDesign.netlist().find(circuit::Pin(), "inp1");
    CHECK(sta.late_rise_arrival(pin) == slew_type(0));
    CHECK(sta.late_fall_arrival(pin) == slew_type(0));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(1e-11), -13));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(1e-11), -13));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-6.17361e-12), -13));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-9.33019e-11), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "inp2");
    CHECK(sta.late_rise_arrival(pin) == slew_type(0));
    CHECK(sta.late_fall_arrival(pin) == slew_type(0));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(1e-11), -13));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(1e-11), -13));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-6.17361e-12), -13));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-9.33019e-11), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "iccad_clk");
    CHECK(sta.late_rise_arrival(pin) == slew_type(0));
    CHECK(sta.late_fall_arrival(pin) == slew_type(0));
    CHECK(sta.late_rise_slew(pin) == slew_type(0));
    CHECK(sta.late_fall_slew(pin) == slew_type(0));
    CHECK(sta.late_rise_slack(pin) == timing::Optimistic::best());
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-4.85982e-11), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "out");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(8.48581e-11), -13));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(8.48581e-11), -13));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(2.75146e-11), -13));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(2.75146e-11), -13));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-4.85808e-12), -13));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-4.85808e-12), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "u1:a");
    CHECK(sta.late_rise_arrival(pin) == slew_type(0));
    CHECK(sta.late_fall_arrival(pin) == slew_type(0));
    CHECK(sta.late_rise_slew(pin) == slew_type(0));
    CHECK(sta.late_fall_slew(pin) == slew_type(0));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-9.33019e-11), -13));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-6.17361e-12), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "u1:b");
    CHECK(sta.late_rise_arrival(pin) == slew_type(0));
    CHECK(sta.late_fall_arrival(pin) == slew_type(0));
    CHECK(sta.late_rise_slew(pin) == slew_type(0));
    CHECK(sta.late_fall_slew(pin) == slew_type(0));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-9.33019e-11), -13));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-6.17361e-12), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "u1:o");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(2.71815e-11), -13));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(5.43614e-11), -13));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(3.25988e-11), -13));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(6.5232e-11), -13));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-6.17361e-12), -13));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-9.33019e-11), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "u2:o");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(1.73213e-10), -13));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(8.60847e-11), -13));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(1.17564e-10), -13));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(5.87956e-11), -13));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-9.33019e-11), -13));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-6.17361e-12), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "u2:a");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(2.7188e-11), -13));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(5.43679e-11), -13));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(3.25988e-11), -13));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(6.5232e-11), -13));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-6.17361e-12), -13));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-9.33019e-11), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "u2:b");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(2.43556e-11), -13));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(2.43556e-11), -13));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(1.93187e-11), -13));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(1.93187e-11), -13));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(6.94728e-13), -13));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-4.85982e-11), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "f1:d");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(1.73302e-10), -13));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(8.61736e-11), -13));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(1.17564e-10), -13));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(5.87956e-11), -13));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-9.33019e-11), -13));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-6.17361e-12), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "f1:ck");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(5.02413e-12), -13));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(5.02413e-12), -13));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(5.21868e-12), -13));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(5.21868e-12), -13));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-4.85982e-11), -13));
    CHECK(sta.late_fall_slack(pin) == timing::Optimistic::best());

    pin = mDesign.netlist().find(circuit::Pin(), "f1:q");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(2.43428e-11), -13));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(2.43428e-11), -13));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(1.93186e-11), -13));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(1.93186e-11), -13));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-4.85808e-12), -13));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-4.85982e-11), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "u3:a");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(2.43589e-11), -13));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(2.43589e-11), -13));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(1.93187e-11), -13));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(1.93187e-11), -13));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-4.85808e-12), -13));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-4.85808e-12), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "u3:o");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(5.34254e-11), -13));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(5.34254e-11), -13));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(2.77874e-11), -13));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(2.77874e-11), -13));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-4.85808e-12), -13));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-4.85808e-12), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "u4:a");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(5.344e-11), -13));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(5.344e-11), -13));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(2.77875e-11), -13));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(2.77875e-11), -13));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-4.85808e-12), -13));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-4.85808e-12), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "u4:o");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(8.48369e-11), -13));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(8.48369e-11), -13));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(2.75146e-11), -13));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(2.75146e-11), -13));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-4.85808e-12), -13));
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-4.85808e-12), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "lcb1:a");
    CHECK(sta.late_rise_arrival(pin) == slew_type(0));
    CHECK(sta.late_fall_arrival(pin) == slew_type(0));
    CHECK(sta.late_rise_slew(pin) == slew_type(0));
    CHECK(sta.late_fall_slew(pin) == slew_type(0));
    CHECK(sta.late_rise_slack(pin) == timing::Optimistic::best());
    CHECK(diff(sta.late_fall_slack(pin), slew_type(-4.85982e-11), -13));

    pin = mDesign.netlist().find(circuit::Pin(), "lcb1:o");
    CHECK(diff(sta.late_rise_arrival(pin), slew_type(4.99672e-12), -13));
    CHECK(diff(sta.late_fall_arrival(pin), slew_type(4.99672e-12), -13));
    CHECK(diff(sta.late_rise_slew(pin), slew_type(5.21861e-12), -13));
    CHECK(diff(sta.late_fall_slew(pin), slew_type(5.21861e-12), -13));
    CHECK(diff(sta.late_rise_slack(pin), slew_type(-4.85982e-11), -13));
    CHECK(sta.late_fall_slack(pin) == timing::Optimistic::best());

    CHECK(sta.timing_endpoints().size() == 2);
    CHECK(mDesign.netlist().name(*sta.timing_endpoints().begin()) == "out");
    CHECK(mDesign.netlist().name(*(++sta.timing_endpoints().begin())) == "f1:d");

    /* ---- WNS and TNS ---- */
    CHECK(sta.early_tns() == slew_type(0));
    CHECK(diff(sta.early_wns(), slew_type(5.16088e-11), -13));
    CHECK(diff(sta.late_tns(), slew_type(-9.816e-11), -13));
    CHECK(diff(sta.late_wns(), slew_type(-9.33019e-11), -13));
}

