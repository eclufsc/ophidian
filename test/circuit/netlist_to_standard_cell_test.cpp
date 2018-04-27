#include <ophidian/circuit/Netlist.h>

#include <catch.hpp>

namespace
{
    class NetlistAndStdCellFixture
    {
    public:
        ophidian::circuit::CellInstance       cell1, cell2;
        ophidian::circuit::PinInstance        pin1, pin2;
        ophidian::circuit::Cell stdCell1, stdCell2;
        ophidian::circuit::Pin  stdCell3, stdCell4;

        ophidian::circuit::Netlist             netlist;
        ophidian::circuit::StandardCells stdCells;

        NetlistAndStdCellFixture()
        {
            cell1 = netlist.add(ophidian::circuit::CellInstance(), "cell1");
            cell2 = netlist.add(ophidian::circuit::CellInstance(), "cell2");

            pin1 = netlist.add(ophidian::circuit::PinInstance(), "pin1");
            pin2 = netlist.add(ophidian::circuit::PinInstance(), "pin2");

            stdCell1 = stdCells.add(ophidian::circuit::Cell(), "INVX1");
            stdCell2 = stdCells.add(ophidian::circuit::Cell(), "INVX2");
            stdCell3 = stdCells.add(
                ophidian::circuit::Pin(),
                "INVX1_in",
                ophidian::circuit::PinDirection::INPUT);
            stdCell4 = stdCells.add(
                ophidian::circuit::Pin(),
                "INVX2_out",
                ophidian::circuit::PinDirection::OUTPUT);
        }
    };
}

TEST_CASE("LibraryMapping: setting the standard cell of a cell","[library_mapping][cell]")
{
    NetlistAndStdCellFixture netlistAndStdCellFixture;

    netlistAndStdCellFixture.netlist.cellStdCell(netlistAndStdCellFixture.cell1, netlistAndStdCellFixture.stdCell1);
    netlistAndStdCellFixture.netlist.cellStdCell(netlistAndStdCellFixture.cell2, netlistAndStdCellFixture.stdCell2);

    REQUIRE(
        netlistAndStdCellFixture.netlist.cellStdCell(
            netlistAndStdCellFixture.cell1) == netlistAndStdCellFixture.stdCell1);
    REQUIRE(
        netlistAndStdCellFixture.netlist.cellStdCell(
            netlistAndStdCellFixture.cell2) == netlistAndStdCellFixture.stdCell2);
    REQUIRE(
        netlistAndStdCellFixture.netlist.cellStdCell(netlistAndStdCellFixture.cell1) !=
        netlistAndStdCellFixture.netlist.cellStdCell(netlistAndStdCellFixture.cell2));
}

TEST_CASE("LibraryMapping: setting the standard cell of a pin", "[library_mapping][pin]")
{
    NetlistAndStdCellFixture netlistAndStdCellFixture;

    netlistAndStdCellFixture.netlist.pinStdCell(netlistAndStdCellFixture.pin1, netlistAndStdCellFixture.stdCell3);
    netlistAndStdCellFixture.netlist.pinStdCell(netlistAndStdCellFixture.pin2, netlistAndStdCellFixture.stdCell4);

    REQUIRE(
        netlistAndStdCellFixture.netlist.pinStdCell(
            netlistAndStdCellFixture.pin1) == netlistAndStdCellFixture.stdCell3);
    REQUIRE(
        netlistAndStdCellFixture.netlist.pinStdCell(
            netlistAndStdCellFixture.pin2) == netlistAndStdCellFixture.stdCell4);
    REQUIRE(
        netlistAndStdCellFixture.netlist.pinStdCell(netlistAndStdCellFixture.pin1) !=
        netlistAndStdCellFixture.netlist.pinStdCell(netlistAndStdCellFixture.pin2));
}
