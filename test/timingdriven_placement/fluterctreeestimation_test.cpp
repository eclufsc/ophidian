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

using namespace ophidian;

using netlist_type = circuit::Netlist;
using standard_cells_type = standard_cell::StandardCells;
using library_mapping_type = circuit::LibraryMapping;
using placement_type = placement::Placement;
using placement_mapping_type = placement::PlacementMapping;
using placement_library_type = placement::Library;

using cell_type = circuit::Cell;
using pin_type  = circuit::Pin;
using net_type  = circuit::Net;

using std_cell_type = standard_cell::Cell;
using std_pin_type  = standard_cell::Pin;

using location_dbu = util::LocationDbu;

using rctree_type = timingdriven_placement::RCTree;
using rctree_builder_type = timingdriven_placement::FluteRCTreeBuilder;
using capacitance_unit_type = rctree_type::capacitance_unit_type;
using resistance_unit_type  = rctree_type::resistance_unit_type;

bool compare(capacitance_unit_type a, capacitance_unit_type b)
{
    if(((a-b)/b) >= 0.001)
        std::cout << a << " and " << b << " are different ( " << ((a-b)/b)*100 << "% )" << std::endl;
    return ((a-b)/b) < 0.001;
}

namespace {
class FluteRCTreeFixture {
public:
    cell_type mCell_u1, mCell_u2, mCell_u3, mCell_u4;
    pin_type mPin_u1o, mPin_u2a, mPin_u3a, mPin_u4a;
    net_type mNet;
    std_cell_type mStdCell_u1, mStdCell_u2, mStdCell_u3, mStdCell_u4;
    std_pin_type mStdPin_u1o, mStdPin_u2a, mStdPin_u3a, mStdPin_u4a;

    netlist_type mNetlist;
    standard_cells_type mStdCells;
    library_mapping_type mLibraryMapping;

    placement_type mPlacement;
    placement_library_type mPlacementLibrary;
    placement_mapping_type mPlacementMapping;

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

        mCell_u1 = mNetlist.add(cell_type(), "mCell_u1");
        mCell_u2 = mNetlist.add(cell_type(), "mCell_u2");
        mCell_u3 = mNetlist.add(cell_type(), "mCell_u3");
        mCell_u4 = mNetlist.add(cell_type(), "mCell_u4");

        mPin_u1o = mNetlist.add(pin_type(), "mCell_u1:mPin_u1o");
        mPin_u2a = mNetlist.add(pin_type(), "mCell_u2:mPin_u2a");
        mPin_u3a = mNetlist.add(pin_type(), "mCell_u3:mPin_u3a");
        mPin_u4a = mNetlist.add(pin_type(), "mCell_u4:mPin_u4a");

        mNet = mNetlist.add(net_type(), "net");

        mNetlist.add(mCell_u1, mPin_u1o);
        mNetlist.add(mCell_u2, mPin_u2a);
        mNetlist.add(mCell_u3, mPin_u3a);
        mNetlist.add(mCell_u4, mPin_u4a);

        mStdCell_u1 = mStdCells.find(std_cell_type(), "mStdCell_u1");
        mStdCell_u2 = mStdCells.find(std_cell_type(), "mStdCell_u2");
        mStdCell_u3 = mStdCells.find(std_cell_type(), "mStdCell_u3");
        mStdCell_u4 = mStdCells.find(std_cell_type(), "mStdCell_u4");

        mStdPin_u1o = mStdCells.find(std_pin_type(), "mStdCell_u1:o");
        mStdPin_u2a = mStdCells.find(std_pin_type(), "mStdCell_u2:a");
        mStdPin_u3a = mStdCells.find(std_pin_type(), "mStdCell_u3:a");
        mStdPin_u4a = mStdCells.find(std_pin_type(), "mStdCell_u4:a");

        mLibraryMapping.cellStdCell(mCell_u1, mStdCell_u1);
        mLibraryMapping.cellStdCell(mCell_u2, mStdCell_u2);
        mLibraryMapping.cellStdCell(mCell_u3, mStdCell_u3);
        mLibraryMapping.cellStdCell(mCell_u4, mStdCell_u4);

        mLibraryMapping.pinStdCell(mPin_u1o, mStdPin_u1o);
        mLibraryMapping.pinStdCell(mPin_u2a, mStdPin_u2a);
        mLibraryMapping.pinStdCell(mPin_u3a, mStdPin_u3a);
        mLibraryMapping.pinStdCell(mPin_u4a, mStdPin_u4a);

        mPlacement.placeCell(mCell_u1, location_dbu(2000 - pinXOutputOffset, 2000 - pinYOutputOffset));
        mPlacement.placeCell(mCell_u2, location_dbu(3000 - pinXInputOffset, 4000 - pinYInputOffset));
        mPlacement.placeCell(mCell_u3, location_dbu(5000 - pinXInputOffset, 1000 - pinYInputOffset));
        mPlacement.placeCell(mCell_u4, location_dbu(5000 - pinXInputOffset, 5000 - pinYInputOffset));

        mTimingLibrary.reset(new ophidian::timing::Library(mStdCells, mTimingArcs));
        mTimingLibrary->init(*mLiberty, false);
    }

};
}

TEST_CASE_METHOD(FluteRCTreeFixture, "Flute RCTree: init", "[timingdriven_placement][flute][rctree]")
{
    rctree_builder_type builder;
    CHECK(mNetlist.size(cell_type()) == 4);
    CHECK(mStdCells.size(std_cell_type()) == 4);
    CHECK(mLef.get()->databaseUnits() == 1000);
    CHECK(mLef.get()->macros().front().name == "mStdCell_u1");
    CHECK(mLiberty.get()->cells.size() == 4);
    CHECK(mTimingLibrary->capacitance(mStdPin_u1o) == capacitance_unit_type(0.0));
    CHECK(mTimingLibrary->capacitance(mStdPin_u2a) == capacitance_unit_type(1e-15));
    CHECK(mTimingLibrary->capacitance(mStdPin_u3a) == capacitance_unit_type(1e-15));
    CHECK(mTimingLibrary->capacitance(mStdPin_u4a) == capacitance_unit_type(1e-15));
}

TEST_CASE_METHOD(FluteRCTreeFixture, "Flute RCTree: RCTree for one pins", "[timingdriven_placement][flute][rctree]")
{
    mPlacement.placeCell(mCell_u1, location_dbu(100000 - pinXOutputOffset, 100000 - pinYOutputOffset));

    mNetlist.connect(mNet, mPin_u1o);

    rctree_type rctree;
    rctree_builder_type builder;

    rctree_type::capacitor_type sourceCap = builder.build(mPlacement, mPlacementMapping, mLibraryMapping, mNetlist, *mTimingLibrary, *mLef.get(), mNet, rctree, mPin_u1o);

    CHECK(lemon::connected(rctree.g()));
    CHECK(rctree.size(rctree_type::capacitor_type()) == 2);

    CHECK(rctree.capacitance(sourceCap) == mTimingLibrary->capacitance(mStdPin_u1o));
    CHECK(rctree.lumped() == mTimingLibrary->capacitance(mStdPin_u1o));
    CHECK(rctree.resistance(rctree.resistors(sourceCap)) == resistance_unit_type(0.0));
}

TEST_CASE_METHOD(FluteRCTreeFixture, "Flute RCTree: RCTree for two pins", "[timingdriven_placement][flute][rctree]")
{
    mNetlist.connect(mNet, mPin_u1o);
    mNetlist.connect(mNet, mPin_u2a);

    rctree_type rctree;
    rctree_builder_type builder;

    rctree_type::capacitor_type sourceCap = builder.build(mPlacement, mPlacementMapping, mLibraryMapping, mNetlist, *mTimingLibrary, *mLef.get(), mNet, rctree, mPin_u1o);

    REQUIRE(lemon::connected(rctree.g()));
    REQUIRE(rctree.size(rctree_type::capacitor_type()) == 4);
    REQUIRE(rctree.size(rctree_type::resistor_type()) == 3);

    CHECK(compare(rctree.lumped(), capacitance_unit_type(1.0) + ophidian::util::femtofarad_t(0.6)));

    rctree_type::capacitor_type cap_u1o = rctree.capacitor(mNetlist.name(mPin_u1o));
    rctree_type::capacitor_type cap_u2a  = rctree.capacitor(mNetlist.name(mPin_u2a));

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

    rctree_type rctree;
    rctree_builder_type builder;

    rctree_type::capacitor_type sourceCap = builder.build(mPlacement, mPlacementMapping, mLibraryMapping, mNetlist, *mTimingLibrary, *mLef.get(), mNet, rctree, mPin_u1o);

    REQUIRE(rctree.size(rctree_type::capacitor_type()) == 7);
    REQUIRE(rctree.size(rctree_type::resistor_type()) == 6);
    REQUIRE(lemon::connected(rctree.g()));

    CHECK(compare(rctree.lumped(), capacitance_unit_type(2.0) + ophidian::util::femtofarad_t(1.2)));

    rctree_type::capacitor_type cap_u1o = rctree.capacitor(mNetlist.name(mPin_u1o));
    rctree_type::capacitor_type cap_u2a  = rctree.capacitor(mNetlist.name(mPin_u2a));
    rctree_type::capacitor_type cap_u3a  = rctree.capacitor(mNetlist.name(mPin_u3a));

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

    rctree_type rctree;
    rctree_builder_type builder;

    rctree_type::capacitor_type sourceCap = builder.build(mPlacement, mPlacementMapping, mLibraryMapping, mNetlist, *mTimingLibrary, *mLef.get(), mNet, rctree, mPin_u1o);

    REQUIRE(lemon::connected(rctree.g()));
    REQUIRE(rctree.size(rctree_type::capacitor_type()) ==  10);
    REQUIRE(rctree.size(rctree_type::resistor_type()) == 9);

    CHECK(compare(rctree.lumped(), capacitance_unit_type(3.0) + ophidian::util::femtofarad_t(1.8)));

    rctree_type::capacitor_type cap_u1o = rctree.capacitor(mNetlist.name(mPin_u1o));
    rctree_type::capacitor_type cap_u2a  = rctree.capacitor(mNetlist.name(mPin_u2a));
    rctree_type::capacitor_type cap_u3a  = rctree.capacitor(mNetlist.name(mPin_u3a));
    rctree_type::capacitor_type cap_u4a  = rctree.capacitor(mNetlist.name(mPin_u4a));

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
    mPlacement.placeCell(mCell_u1, location_dbu(100000 - pinXOutputOffset, 100000 - pinYOutputOffset));
    mPlacement.placeCell(mCell_u2, location_dbu(200000 - pinXInputOffset, 100000 - pinYInputOffset));

    mNetlist.connect(mNet, mPin_u1o);
    mNetlist.connect(mNet, mPin_u2a);

    rctree_type rctree;
    rctree_builder_type builder;

    rctree_type::capacitor_type sourceCap = builder.build(mPlacement, mPlacementMapping, mLibraryMapping, mNetlist, *mTimingLibrary, *mLef.get(), mNet, rctree, mPin_u1o);

    REQUIRE(lemon::connected(rctree.g()));
    REQUIRE(rctree.size(rctree_type::capacitor_type()) == 4);
}

TEST_CASE_METHOD(FluteRCTreeFixture, "Flute RCTree: Build with two points (different) and no divisions", "[timingdriven_placement][flute][rctree]")
{
    mPlacement.placeCell(mCell_u1, location_dbu(100000 - pinXOutputOffset, 100000 - pinYOutputOffset));
    mPlacement.placeCell(mCell_u2, location_dbu(200001 - pinXInputOffset, 100000 - pinYInputOffset));

    mNetlist.connect(mNet, mPin_u1o);
    mNetlist.connect(mNet, mPin_u2a);

    rctree_type rctree;
    rctree_builder_type builder;

    rctree_type::capacitor_type sourceCap = builder.build(mPlacement, mPlacementMapping, mLibraryMapping, mNetlist, *mTimingLibrary, *mLef.get(), mNet, rctree, mPin_u1o);

    REQUIRE(lemon::connected(rctree.g()));
    REQUIRE(rctree.size(rctree_type::capacitor_type()) == 5);
}

TEST_CASE_METHOD(FluteRCTreeFixture, "Flute RCTree: Build with four points and no divisions", "[timingdriven_placement][flute][rctree]")
{
    mPlacement.placeCell(mCell_u1, location_dbu(200000 - pinXOutputOffset, 200000 - pinYOutputOffset));
    mPlacement.placeCell(mCell_u2, location_dbu(300000 - pinXInputOffset, 400000 - pinYInputOffset));
    mPlacement.placeCell(mCell_u3, location_dbu(500000 - pinXInputOffset, 100000 - pinYInputOffset));
    mPlacement.placeCell(mCell_u4, location_dbu(500000 - pinXInputOffset, 500000 - pinYInputOffset));

    mNetlist.connect(mNet, mPin_u1o);
    mNetlist.connect(mNet, mPin_u2a);
    mNetlist.connect(mNet, mPin_u3a);
    mNetlist.connect(mNet, mPin_u4a);

    rctree_type rctree;
    rctree_builder_type builder;

    rctree_type::capacitor_type sourceCap = builder.build(mPlacement, mPlacementMapping, mLibraryMapping, mNetlist, *mTimingLibrary, *mLef.get(), mNet, rctree, mPin_u1o);

    REQUIRE(lemon::connected(rctree.g()));
    REQUIRE(rctree.size(rctree_type::capacitor_type()) ==  14);
}
