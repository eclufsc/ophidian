#include <catch.hpp>

#include <boost/geometry.hpp>

#include <ophidian/placement/Placement.h>

using namespace ophidian::placement;
using namespace ophidian::circuit;

using dbu_type = Library::std_cell_geometry_type::unit_type;
class NetlistFixture {
public:
    Cell stdCell1;

    Pin pinType1;
    Pin pinType2;

    CellInstance cell1, cell2;

    PinInstance pin1, pin2, pin3, pin4, pin5, pin6;

    Input input1, input2;

    Output output1, output2;

    StandardCells stdCells;
    Netlist netlist;
    Library library{stdCells};

    NetlistFixture() {
        stdCell1 = stdCells.add_cell("invX1");
        pinType1 = stdCells.add_pin("invX1:a", PinDirection::INPUT);
        pinType2 = stdCells.add_pin("invX1:o", PinDirection::OUTPUT);

        auto cellBox = Library::std_cell_geometry_type::box_type{{dbu_type{0}, dbu_type{0}}, 
        {dbu_type{10}, dbu_type{10}}};
        auto cellGeometry = Library::std_cell_geometry_type{{cellBox}};

        library.geometry(stdCell1) = cellGeometry;

        library.offset(pinType1) = {dbu_type{1}, dbu_type{2}};
        library.offset(pinType2) = {dbu_type{2}, dbu_type{1}};

        cell1 = netlist.add_cell_instance("cell1");
        cell2 = netlist.add_cell_instance("cell2");

        netlist.connect(cell1, stdCell1);
        netlist.connect(cell2, stdCell1);

        pin1 = netlist.add_pin_instance("pin1");
        pin2 = netlist.add_pin_instance("pin2");
        pin3 = netlist.add_pin_instance("pin3");
        pin4 = netlist.add_pin_instance("pin4");
        pin5 = netlist.add_pin_instance("pin5");
        pin6 = netlist.add_pin_instance("pin6");

        netlist.connect(pin5, pinType1);
        netlist.connect(pin6, pinType2);

        input1 = netlist.add_input_pad(pin1);
        input2 = netlist.add_input_pad(pin2);

        output1 = netlist.add_output_pad(pin3);
        output2 = netlist.add_output_pad(pin4);

        netlist.connect(cell1, pin5);
        netlist.connect(cell1, pin6);
    }
};

TEST_CASE_METHOD(NetlistFixture, "Placement: placing a cell", "[placement]") {
    auto placement = Placement{netlist, library};

    auto cell1_location = Placement::point_type{
        Placement::unit_type{10},
        Placement::unit_type{20}
    };

    placement.place(cell1, cell1_location);

    auto placed_cell1_location = placement.location(cell1);

    REQUIRE(cell1_location.x() == placed_cell1_location.x());
    REQUIRE(cell1_location.y() == placed_cell1_location.y());

    auto cell2_location = Placement::point_type{
        Placement::unit_type{5},
        Placement::unit_type{15}
    };

    placement.place(cell2, cell2_location);

    auto placed_cell2_location = placement.location(cell2);

    REQUIRE(cell2_location.x() == placed_cell2_location.x());
    REQUIRE(cell2_location.y() == placed_cell2_location.y());

    REQUIRE(placed_cell1_location.x() != placed_cell2_location.x());
    REQUIRE(placed_cell1_location.y() != placed_cell2_location.y());
}

TEST_CASE_METHOD(NetlistFixture, "Placement: placing an input pad", "[placement]") {
    auto placement = Placement{netlist, library};

    auto input1Location = Placement::point_type{
        Placement::unit_type{10},
        Placement::unit_type{20}
    };
    placement.place(input1, input1Location);

    REQUIRE(placement.location(input1).x() == input1Location.x());
    REQUIRE(placement.location(input1).y() == input1Location.y());
    REQUIRE(placement.location(pin1).x() == input1Location.x());
    REQUIRE(placement.location(pin1).y() == input1Location.y());

    auto input2Location = Placement::point_type{
        Placement::unit_type{5},
        Placement::unit_type{15}
    };

    placement.place(input2, input2Location);

    REQUIRE(placement.location(input2).x() == input2Location.x());
    REQUIRE(placement.location(input2).y() == input2Location.y());
    REQUIRE(placement.location(pin2).x() == input2Location.x());
    REQUIRE(placement.location(pin2).y() == input2Location.y());

    REQUIRE(placement.location(input1).x() != placement.location(input2).x());
    REQUIRE(placement.location(input1).y() != placement.location(input2).y());
}

TEST_CASE_METHOD(NetlistFixture, "Placement: placing an output pad", "[placement]") {
    auto placement = Placement{netlist, library};

    auto output1Location = Placement::point_type{
        Placement::unit_type{10},
        Placement::unit_type{20}
    };

    placement.place(output1, output1Location);

    REQUIRE(placement.location(output1).x() == output1Location.x());
    REQUIRE(placement.location(output1).y() == output1Location.y());
    REQUIRE(placement.location(pin3).x() == output1Location.x());
    REQUIRE(placement.location(pin3).y() == output1Location.y());

    auto output2Location = Placement::point_type{
        Placement::unit_type{5},
        Placement::unit_type{15}
    };

    placement.place(output2, output2Location);

    REQUIRE(placement.location(output2).x() == output2Location.x());
    REQUIRE(placement.location(output2).y() == output2Location.y());
    REQUIRE(placement.location(pin4).x() == output2Location.x());
    REQUIRE(placement.location(pin4).y() == output2Location.y());

    REQUIRE(placement.location(output1).x() != placement.location(output2).x());
    REQUIRE(placement.location(output1).y() != placement.location(output2).y());
}

TEST_CASE_METHOD(NetlistFixture, "Placement: get cell geometry", "[placement]") {
    auto placement = Placement{netlist, library};

    auto cellBox = Library::std_cell_geometry_type::box_type{{dbu_type{0}, dbu_type{0}}, 
        {dbu_type{10}, dbu_type{10}}};
    auto cellGeometry = Placement::cell_geometry_type{ {cellBox} };

    REQUIRE(placement.geometry(cell1) == cellGeometry);
    REQUIRE(placement.geometry(cell2) == cellGeometry);

    placement.place(cell1, {dbu_type{10}, dbu_type{20}});

    auto newCellBox = Library::std_cell_geometry_type::box_type{{dbu_type{10}, dbu_type{20}}, 
        {dbu_type{20}, dbu_type{30}}};
    auto newCellGeometry = Placement::cell_geometry_type{ {newCellBox} };

    REQUIRE(placement.geometry(cell1) == newCellGeometry);
}
