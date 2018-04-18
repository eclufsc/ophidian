#include <ophidian/circuit/LibraryMapping.h>

#include <catch.hpp>

namespace
{
    class NetlistAndStdCellFixture
    {
    public:
        ophidian::circuit::Cell       cell1, cell2;
        ophidian::circuit::Pin        pin1, pin2;
        ophidian::standard_cell::Cell stdCell1, stdCell2;
        ophidian::standard_cell::Pin  stdCell3, stdCell4;

        ophidian::circuit::Netlist             netlist;
        ophidian::standard_cell::StandardCells stdCells;

        NetlistAndStdCellFixture()
        {
            cell1 = netlist.add(ophidian::circuit::Cell(), "cell1");
            cell2 = netlist.add(ophidian::circuit::Cell(), "cell2");

            pin1 = netlist.add(ophidian::circuit::Pin(), "pin1");
            pin2 = netlist.add(ophidian::circuit::Pin(), "pin2");

            stdCell1 = stdCells.add(ophidian::standard_cell::Cell(), "INVX1");
            stdCell2 = stdCells.add(ophidian::standard_cell::Cell(), "INVX2");
            stdCell3 = stdCells.add(
                ophidian::standard_cell::Pin(),
                "INVX1_in",
                ophidian::standard_cell::PinDirection::INPUT);
            stdCell4 = stdCells.add(
                ophidian::standard_cell::Pin(),
                "INVX2_out",
                ophidian::standard_cell::PinDirection::OUTPUT);
        }
    };
}

TEST_CASE("LibraryMapping: setting the standard cell of a cell","[library_mapping][cell]")
{
    NetlistAndStdCellFixture netlistAndStdCellFixture;

    ophidian::circuit::LibraryMapping libraryMapping(netlistAndStdCellFixture.netlist);

    libraryMapping.cellStdCell(netlistAndStdCellFixture.cell1, netlistAndStdCellFixture.stdCell1);
    libraryMapping.cellStdCell(netlistAndStdCellFixture.cell2, netlistAndStdCellFixture.stdCell2);

    REQUIRE(
        libraryMapping.cellStdCell(
            netlistAndStdCellFixture.cell1) == netlistAndStdCellFixture.stdCell1);
    REQUIRE(
        libraryMapping.cellStdCell(
            netlistAndStdCellFixture.cell2) == netlistAndStdCellFixture.stdCell2);
    REQUIRE(
        libraryMapping.cellStdCell(netlistAndStdCellFixture.cell1) !=
        libraryMapping.cellStdCell(netlistAndStdCellFixture.cell2));
}

TEST_CASE("LibraryMapping: setting the standard cell of a pin", "[library_mapping][pin]")
{
    NetlistAndStdCellFixture netlistAndStdCellFixture;

    ophidian::circuit::LibraryMapping libraryMapping(netlistAndStdCellFixture.netlist);

    libraryMapping.pinStdCell(netlistAndStdCellFixture.pin1, netlistAndStdCellFixture.stdCell3);
    libraryMapping.pinStdCell(netlistAndStdCellFixture.pin2, netlistAndStdCellFixture.stdCell4);

    REQUIRE(
        libraryMapping.pinStdCell(
            netlistAndStdCellFixture.pin1) == netlistAndStdCellFixture.stdCell3);
    REQUIRE(
        libraryMapping.pinStdCell(
            netlistAndStdCellFixture.pin2) == netlistAndStdCellFixture.stdCell4);
    REQUIRE(
        libraryMapping.pinStdCell(netlistAndStdCellFixture.pin1) !=
        libraryMapping.pinStdCell(netlistAndStdCellFixture.pin2));
}
