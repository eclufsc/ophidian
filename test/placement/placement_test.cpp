#include <catch.hpp>

#include <boost/geometry.hpp>

#include <ophidian/placement/Placement.h>

using namespace ophidian::placement;
using namespace ophidian::circuit;

class NetlistFixture {
public:
    CellInstance cell1, cell2;

    PinInstance pin1, pin2, pin3, pin4;

    Input input1, input2;

    Output output1, output2;

    Netlist netlist;
    Library library{StandardCells{}};

    Placement::point_type location_1{Placement::unit_type{10}, Placement::unit_type{20}};

    Placement::point_type location_2{Placement::unit_type{5}, Placement::unit_type{15}};

    NetlistFixture() {
        cell1 = netlist.add_cell_instance("cell1");
        cell2 = netlist.add_cell_instance("cell2");

        pin1 = netlist.add_pin_instance("pin1");
        pin2 = netlist.add_pin_instance("pin2");
        pin3 = netlist.add_pin_instance("pin3");
        pin4 = netlist.add_pin_instance("pin4");

        input1 = netlist.add_input_pad(pin1);
        input2 = netlist.add_input_pad(pin2);

        output1 = netlist.add_output_pad(pin3);
        output2 = netlist.add_output_pad(pin4);
    }
};

class PlacementFixture : public NetlistFixture{
public:
    Placement placement;
    PlacementFixture():placement(netlist, library){}
};

TEST_CASE_METHOD(PlacementFixture, "Placement: placing a cell1 with north orientation", "[placement]") {
    auto cell1_orientation = Placement::orientation_type::N;
    placement.place(cell1, location_1);
    placement.setOrientation(cell1, cell1_orientation);

    REQUIRE(boost::geometry::equals(placement.location(cell1), location_1));
    REQUIRE(placement.orientation(cell1) == cell1_orientation);
}

TEST_CASE_METHOD(PlacementFixture, "Placement: placing a cell2 with fliped south orientation", "[placement]") {
    auto cell2_orientation = Placement::orientation_type::FS;
    placement.place(cell2, location_2);
    placement.setOrientation(cell2, cell2_orientation);

    REQUIRE(boost::geometry::equals(placement.location(cell2), location_2));
    REQUIRE(placement.orientation(cell2) == cell2_orientation);
}

TEST_CASE_METHOD(PlacementFixture, "Placement: placing an input1 pad", "[placement]") {
    placement.place(input1, location_1);
    REQUIRE(boost::geometry::equals(placement.location(input1), location_1));
}


TEST_CASE_METHOD(PlacementFixture, "Placement: placing an input2 pad", "[placement]") {
    placement.place(input2, location_2);
    REQUIRE(boost::geometry::equals(placement.location(input2), location_2));
}

TEST_CASE_METHOD(PlacementFixture, "Placement: placing an output1 pad", "[placement]") {
    placement.place(output1, location_1);
    REQUIRE(boost::geometry::equals(placement.location(output1), location_1));
}

TEST_CASE_METHOD(PlacementFixture, "Placement: placing an output2 pad", "[placement]") {
    placement.place(output2, location_2);
    REQUIRE(boost::geometry::equals(placement.location(output2), location_2));
}

TEST_CASE_METHOD(PlacementFixture, "Placement: checking whether a cell's position is fixed", "[placement]") {
    REQUIRE(placement.isFixed(cell1) == false);
}

TEST_CASE_METHOD(PlacementFixture, "Placement: fixing a cell position", "[placement]") {
    placement.fixLocation(cell1);
    REQUIRE(placement.isFixed(cell1) == true);
}
