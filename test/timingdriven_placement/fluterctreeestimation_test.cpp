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

#include <lemon/connectivity.h>
#include <ophidian/timingdriven_placement/FluteRCTreeEstimation.h>
#include <ophidian/placement/Lef2Library.h>

using namespace ophidian::timingdriven_placement;

bool compare(ophidian::util::farad_t a, ophidian::util::farad_t b)
{
    if(((a-b)/b) >= 0.001)
        std::cout << a << " and " << b << " are different ( " << ((a-b)/b)*100 << "% )" << std::endl;
    return ((a-b)/b) < 0.001;
}

namespace {
class FluteRCTreeFixture {
public:
    ophidian::circuit::Cell mCell_u1, mCell_u2, mCell_u3, mCell_u4;
    ophidian::circuit::Pin mPin_u1o, mPin_u2a, mPin_u3a, mPin_u4a;
    ophidian::circuit::Net mNet;
    ophidian::standard_cell::Cell mStdCell_u1, mStdCell_u2, mStdCell_u3, mStdCell_u4;
    ophidian::standard_cell::Pin mStdPin_u1o, mStdPin_u2a, mStdPin_u3a, mStdPin_u4a;

    ophidian::circuit::Netlist mNetlist;
    ophidian::standard_cell::StandardCells mStdCells;
    ophidian::circuit::LibraryMapping mLibraryMapping;

    ophidian::placement::Placement mPlacement;
    ophidian::placement::Library mPlacementLibrary;
    ophidian::placement::PlacementMapping mPlacementMapping;

    std::unique_ptr<ophidian::parser::Lef> mLef;
    std::shared_ptr<ophidian::parser::Liberty> mLiberty;
    ophidian::timing::TimingArcs mTimingArcs;
    std::shared_ptr<ophidian::timing::Library> mTimingLibrary;

    double pinXOutputOffset, pinYOutputOffset, pinXInputOffset, pinYInputOffset;

    FluteRCTreeFixture() :
        mLibraryMapping(mNetlist),
        mPlacement(mNetlist),
        mPlacementLibrary(mStdCells),
        mPlacementMapping(mPlacement, mPlacementLibrary, mNetlist, mLibraryMapping),
        mTimingArcs(mStdCells),
        mLef(std::make_unique<ophidian::parser::Lef>()),
        mLiberty(ophidian::parser::LibertyParser().readFile("input_files/rctree_files/sample_Late_rctree.lib")),
        pinXOutputOffset(512.5),
        pinYOutputOffset(215),
        pinXInputOffset(275),
        pinYInputOffset(372.5)
    {
        ophidian::parser::LefParser().readFile("input_files/rctree_files/simple_rctree.lef", mLef);
        ophidian::placement::lef2Library(*mLef.get(), mPlacementLibrary, mStdCells);

        mCell_u1 = mNetlist.add(ophidian::circuit::Cell(), "mCell_u1");
        mCell_u2 = mNetlist.add(ophidian::circuit::Cell(), "mCell_u2");
        mCell_u3 = mNetlist.add(ophidian::circuit::Cell(), "mCell_u3");
        mCell_u4 = mNetlist.add(ophidian::circuit::Cell(), "mCell_u4");

        mPin_u1o = mNetlist.add(ophidian::circuit::Pin(), "mCell_u1:mPin_u1o");
        mPin_u2a = mNetlist.add(ophidian::circuit::Pin(), "mCell_u2:mPin_u2a");
        mPin_u3a = mNetlist.add(ophidian::circuit::Pin(), "mCell_u3:mPin_u3a");
        mPin_u4a = mNetlist.add(ophidian::circuit::Pin(), "mCell_u4:mPin_u4a");

        mNet = mNetlist.add(ophidian::circuit::Net(), "net");

        mNetlist.add(mCell_u1, mPin_u1o);
        mNetlist.add(mCell_u2, mPin_u2a);
        mNetlist.add(mCell_u3, mPin_u3a);
        mNetlist.add(mCell_u4, mPin_u4a);

        mStdCell_u1 = mStdCells.find(ophidian::standard_cell::Cell(), "mStdCell_u1");
        mStdCell_u2 = mStdCells.find(ophidian::standard_cell::Cell(), "mStdCell_u2");
        mStdCell_u3 = mStdCells.find(ophidian::standard_cell::Cell(), "mStdCell_u3");
        mStdCell_u4 = mStdCells.find(ophidian::standard_cell::Cell(), "mStdCell_u4");

        mStdPin_u1o = mStdCells.find(ophidian::standard_cell::Pin(), "mStdCell_u1:o");
        mStdPin_u2a = mStdCells.find(ophidian::standard_cell::Pin(), "mStdCell_u2:a");
        mStdPin_u3a = mStdCells.find(ophidian::standard_cell::Pin(), "mStdCell_u3:a");
        mStdPin_u4a = mStdCells.find(ophidian::standard_cell::Pin(), "mStdCell_u4:a");

        mLibraryMapping.cellStdCell(mCell_u1, mStdCell_u1);
        mLibraryMapping.cellStdCell(mCell_u2, mStdCell_u2);
        mLibraryMapping.cellStdCell(mCell_u3, mStdCell_u3);
        mLibraryMapping.cellStdCell(mCell_u4, mStdCell_u4);

        mLibraryMapping.pinStdCell(mPin_u1o, mStdPin_u1o);
        mLibraryMapping.pinStdCell(mPin_u2a, mStdPin_u2a);
        mLibraryMapping.pinStdCell(mPin_u3a, mStdPin_u3a);
        mLibraryMapping.pinStdCell(mPin_u4a, mStdPin_u4a);

        mPlacement.placeCell(mCell_u1, ophidian::util::LocationDbu(2000 - pinXOutputOffset, 2000 - pinYOutputOffset));
        mPlacement.placeCell(mCell_u2, ophidian::util::LocationDbu(3000 - pinXInputOffset, 4000 - pinYInputOffset));
        mPlacement.placeCell(mCell_u3, ophidian::util::LocationDbu(5000 - pinXInputOffset, 1000 - pinYInputOffset));
        mPlacement.placeCell(mCell_u4, ophidian::util::LocationDbu(5000 - pinXInputOffset, 5000 - pinYInputOffset));

        mTimingLibrary.reset(new ophidian::timing::Library(mStdCells, mTimingArcs));
        mTimingLibrary->init(*mLiberty, false);
    }

};
}

TEST_CASE_METHOD(FluteRCTreeFixture, "Flute RCTree: init", "[timingdriven_placement][flute][rctree]")
{
    FluteRCTreeBuilder builder;
    CHECK(mNetlist.size(ophidian::circuit::Cell()) == 4);
    CHECK(mStdCells.size(ophidian::standard_cell::Cell()) == 4);
    CHECK(mLef.get()->databaseUnits() == 1000);
    CHECK(mLef.get()->macros().front().name == "mStdCell_u1");
    CHECK(mLiberty.get()->cells.size() == 4);
    CHECK(mTimingLibrary->capacitance(mStdPin_u1o) == ophidian::util::farad_t(0.0));
    CHECK(mTimingLibrary->capacitance(mStdPin_u2a) == ophidian::util::farad_t(1e-15));
    CHECK(mTimingLibrary->capacitance(mStdPin_u3a) == ophidian::util::farad_t(1e-15));
    CHECK(mTimingLibrary->capacitance(mStdPin_u4a) == ophidian::util::farad_t(1e-15));
}

TEST_CASE_METHOD(FluteRCTreeFixture, "Flute RCTree: RCTree for one pins", "[timingdriven_placement][flute][rctree]")
{
    mPlacement.placeCell(mCell_u1, ophidian::util::LocationDbu(100000 - pinXOutputOffset, 100000 - pinYOutputOffset));

    mNetlist.connect(mNet, mPin_u1o);

    RCTree rctree;
    FluteRCTreeBuilder builder;

    RCTree::capacitor_type sourceCap = builder.build(mPlacement, mPlacementMapping, mLibraryMapping, mNetlist, *mTimingLibrary, *mLef.get(), mNet, rctree, mPin_u1o);

    CHECK(lemon::connected(rctree.g()));
    CHECK(rctree.size(RCTree::capacitor_type()) == 2);

    CHECK(rctree.capacitance(sourceCap) == mTimingLibrary->capacitance(mStdPin_u1o));
    CHECK(rctree.lumped() == mTimingLibrary->capacitance(mStdPin_u1o));
    CHECK(rctree.resistance(rctree.resistors(sourceCap)) == ophidian::util::ohm_t(0.0));
}

TEST_CASE_METHOD(FluteRCTreeFixture, "Flute RCTree: RCTree for two pins", "[timingdriven_placement][flute][rctree]")
{
    mNetlist.connect(mNet, mPin_u1o);
    mNetlist.connect(mNet, mPin_u2a);

    RCTree rctree;
    FluteRCTreeBuilder builder;

    RCTree::capacitor_type sourceCap = builder.build(mPlacement, mPlacementMapping, mLibraryMapping, mNetlist, *mTimingLibrary, *mLef.get(), mNet, rctree, mPin_u1o);

    REQUIRE(lemon::connected(rctree.g()));
    REQUIRE(rctree.size(RCTree::capacitor_type()) == 4);
    REQUIRE(rctree.size(RCTree::resistor_type()) == 3);

    CHECK(compare(rctree.lumped(), ophidian::util::farad_t(1.0) + ophidian::util::femtofarad_t(0.6)));

    RCTree::capacitor_type cap_u1o = rctree.capacitor(mNetlist.name(mPin_u1o));
    RCTree::capacitor_type cap_u2a  = rctree.capacitor(mNetlist.name(mPin_u2a));

    CHECK(rctree.capacitance(sourceCap) == mTimingLibrary->capacitance(mStdPin_u1o));
    CHECK(rctree.capacitance(cap_u1o) == mTimingLibrary->capacitance(mStdPin_u1o));
    CHECK(rctree.capacitance(cap_u2a) == mTimingLibrary->capacitance(mStdPin_u2a));

    CHECK(lemon::countOutArcs(rctree.g(), cap_u1o) == 1);
    auto capOpposite_u1o = rctree.oppositeCapacitor(cap_u1o, rctree.resistors(cap_u1o));
    CHECK(compare(rctree.capacitance(capOpposite_u1o), ophidian::util::femtofarad_t(0.3)));

    CHECK(lemon::countOutArcs(rctree.g(), cap_u2a) == 1);
    auto capOpposite_u2a = rctree.oppositeCapacitor(cap_u2a, rctree.resistors(cap_u2a));
    CHECK(compare(rctree.capacitance(capOpposite_u2a), ophidian::util::femtofarad_t(0.3)));

}

TEST_CASE_METHOD(FluteRCTreeFixture, "Flute RCTree: RCTree for three pins", "[timingdriven_placement][flute][rctree]")
{
    mNetlist.connect(mNet, mPin_u1o);
    mNetlist.connect(mNet, mPin_u2a);
    mNetlist.connect(mNet, mPin_u3a);

    RCTree rctree;
    FluteRCTreeBuilder builder;

    RCTree::capacitor_type sourceCap = builder.build(mPlacement, mPlacementMapping, mLibraryMapping, mNetlist, *mTimingLibrary, *mLef.get(), mNet, rctree, mPin_u1o);

    REQUIRE(rctree.size(RCTree::capacitor_type()) == 7);
    REQUIRE(rctree.size(RCTree::resistor_type()) == 6);
    REQUIRE(lemon::connected(rctree.g()));

    CHECK(compare(rctree.lumped(), ophidian::util::farad_t(2.0) + ophidian::util::femtofarad_t(1.2)));

    RCTree::capacitor_type cap_u1o = rctree.capacitor(mNetlist.name(mPin_u1o));
    RCTree::capacitor_type cap_u2a  = rctree.capacitor(mNetlist.name(mPin_u2a));
    RCTree::capacitor_type cap_u3a  = rctree.capacitor(mNetlist.name(mPin_u3a));

    CHECK(rctree.capacitance(sourceCap) == mTimingLibrary->capacitance(mStdPin_u1o));
    CHECK(rctree.capacitance(cap_u1o) == mTimingLibrary->capacitance(mStdPin_u1o));
    CHECK(rctree.capacitance(cap_u2a) == mTimingLibrary->capacitance(mStdPin_u2a));
    CHECK(rctree.capacitance(cap_u3a) == mTimingLibrary->capacitance(mStdPin_u3a));

    CHECK(lemon::countOutArcs(rctree.g(), cap_u1o) == 1);
    auto capOpposite_u1o = rctree.oppositeCapacitor(cap_u1o, rctree.resistors(cap_u1o));
    CHECK(compare(rctree.capacitance(capOpposite_u1o), ophidian::util::femtofarad_t(0.1)));

    CHECK(lemon::countOutArcs(rctree.g(), cap_u2a) == 1);
    auto capOpposite_u2a = rctree.oppositeCapacitor(cap_u2a, rctree.resistors(cap_u2a));
    CHECK(compare(rctree.capacitance(capOpposite_u2a), ophidian::util::femtofarad_t(0.2)));

    CHECK(lemon::countOutArcs(rctree.g(), cap_u3a) == 1);
    auto capOpposite_u3a = rctree.oppositeCapacitor(cap_u3a, rctree.resistors(cap_u3a));
    CHECK(compare(rctree.capacitance(capOpposite_u3a), ophidian::util::femtofarad_t(0.3)));

}

TEST_CASE_METHOD(FluteRCTreeFixture, "Flute RCTree: RCTree for four pins", "[timingdriven_placement][flute][rctree]")
{
    mNetlist.connect(mNet, mPin_u1o);
    mNetlist.connect(mNet, mPin_u2a);
    mNetlist.connect(mNet, mPin_u3a);
    mNetlist.connect(mNet, mPin_u4a);

    RCTree rctree;
    FluteRCTreeBuilder builder;

    RCTree::capacitor_type sourceCap = builder.build(mPlacement, mPlacementMapping, mLibraryMapping, mNetlist, *mTimingLibrary, *mLef.get(), mNet, rctree, mPin_u1o);

    REQUIRE(lemon::connected(rctree.g()));
    REQUIRE(rctree.size(RCTree::capacitor_type()) ==  10);
    REQUIRE(rctree.size(RCTree::resistor_type()) == 9);

    CHECK(compare(rctree.lumped(), ophidian::util::farad_t(3.0) + ophidian::util::femtofarad_t(1.8)));

    RCTree::capacitor_type cap_u1o = rctree.capacitor(mNetlist.name(mPin_u1o));
    RCTree::capacitor_type cap_u2a  = rctree.capacitor(mNetlist.name(mPin_u2a));
    RCTree::capacitor_type cap_u3a  = rctree.capacitor(mNetlist.name(mPin_u3a));
    RCTree::capacitor_type cap_u4a  = rctree.capacitor(mNetlist.name(mPin_u4a));

    CHECK(rctree.capacitance(sourceCap) == mTimingLibrary->capacitance(mStdPin_u1o));
    CHECK(rctree.capacitance(cap_u1o) == mTimingLibrary->capacitance(mStdPin_u1o));
    CHECK(rctree.capacitance(cap_u2a) == mTimingLibrary->capacitance(mStdPin_u2a));
    CHECK(rctree.capacitance(cap_u3a) == mTimingLibrary->capacitance(mStdPin_u3a));
    CHECK(rctree.capacitance(cap_u4a) == mTimingLibrary->capacitance(mStdPin_u4a));

    CHECK(lemon::countOutArcs(rctree.g(), cap_u1o) == 1);
    auto capOpposite_u1o = rctree.oppositeCapacitor(cap_u1o, rctree.resistors(cap_u1o));
    CHECK(compare(rctree.capacitance(capOpposite_u1o), ophidian::util::femtofarad_t(0.3)));

    CHECK(lemon::countOutArcs(rctree.g(), cap_u2a) == 1);
    auto capOpposite_u2a = rctree.oppositeCapacitor(cap_u2a, rctree.resistors(cap_u2a));
    CHECK(compare(rctree.capacitance(capOpposite_u2a), ophidian::util::femtofarad_t(0.2)));

    CHECK(lemon::countOutArcs(rctree.g(), cap_u3a) == 1);
    auto capOpposite_u3a = rctree.oppositeCapacitor(cap_u3a, rctree.resistors(cap_u3a));
    CHECK(compare(rctree.capacitance(capOpposite_u3a), ophidian::util::femtofarad_t(0.1)));

    CHECK(lemon::countOutArcs(rctree.g(), cap_u4a) == 1);
    auto capOpposite_u4a = rctree.oppositeCapacitor(cap_u4a, rctree.resistors(cap_u4a));
    CHECK(compare(rctree.capacitance(capOpposite_u4a), ophidian::util::femtofarad_t(0.1)));
}

TEST_CASE_METHOD(FluteRCTreeFixture, "Flute RCTree: Build with two points and no divisions", "[timingdriven_placement][flute][rctree]")
{
    mPlacement.placeCell(mCell_u1, ophidian::util::LocationDbu(100000 - pinXOutputOffset, 100000 - pinYOutputOffset));
    mPlacement.placeCell(mCell_u2, ophidian::util::LocationDbu(200000 - pinXInputOffset, 100000 - pinYInputOffset));

    mNetlist.connect(mNet, mPin_u1o);
    mNetlist.connect(mNet, mPin_u2a);

    RCTree rctree;
    FluteRCTreeBuilder builder;

    RCTree::capacitor_type sourceCap = builder.build(mPlacement, mPlacementMapping, mLibraryMapping, mNetlist, *mTimingLibrary, *mLef.get(), mNet, rctree, mPin_u1o);

    REQUIRE(lemon::connected(rctree.g()));
    REQUIRE(rctree.size(RCTree::capacitor_type()) == 4);
}

TEST_CASE_METHOD(FluteRCTreeFixture, "Flute RCTree: Build with two points (different) and no divisions", "[timingdriven_placement][flute][rctree]")
{
    mPlacement.placeCell(mCell_u1, ophidian::util::LocationDbu(100000 - pinXOutputOffset, 100000 - pinYOutputOffset));
    mPlacement.placeCell(mCell_u2, ophidian::util::LocationDbu(200001 - pinXInputOffset, 100000 - pinYInputOffset));

    mNetlist.connect(mNet, mPin_u1o);
    mNetlist.connect(mNet, mPin_u2a);

    RCTree rctree;
    FluteRCTreeBuilder builder;

    RCTree::capacitor_type sourceCap = builder.build(mPlacement, mPlacementMapping, mLibraryMapping, mNetlist, *mTimingLibrary, *mLef.get(), mNet, rctree, mPin_u1o);

    REQUIRE(lemon::connected(rctree.g()));
    REQUIRE(rctree.size(RCTree::capacitor_type()) == 5);
}

TEST_CASE_METHOD(FluteRCTreeFixture, "Flute RCTree: Build with four points and no divisions", "[timingdriven_placement][flute][rctree]")
{
    mPlacement.placeCell(mCell_u1, ophidian::util::LocationDbu(200000 - pinXOutputOffset, 200000 - pinYOutputOffset));
    mPlacement.placeCell(mCell_u2, ophidian::util::LocationDbu(300000 - pinXInputOffset, 400000 - pinYInputOffset));
    mPlacement.placeCell(mCell_u3, ophidian::util::LocationDbu(500000 - pinXInputOffset, 100000 - pinYInputOffset));
    mPlacement.placeCell(mCell_u4, ophidian::util::LocationDbu(500000 - pinXInputOffset, 500000 - pinYInputOffset));

    mNetlist.connect(mNet, mPin_u1o);
    mNetlist.connect(mNet, mPin_u2a);
    mNetlist.connect(mNet, mPin_u3a);
    mNetlist.connect(mNet, mPin_u4a);

    RCTree rctree;
    FluteRCTreeBuilder builder;

    RCTree::capacitor_type sourceCap = builder.build(mPlacement, mPlacementMapping, mLibraryMapping, mNetlist, *mTimingLibrary, *mLef.get(), mNet, rctree, mPin_u1o);

    REQUIRE(lemon::connected(rctree.g()));
    REQUIRE(rctree.size(RCTree::capacitor_type()) ==  14);
}
