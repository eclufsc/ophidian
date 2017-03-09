#include <catch.hpp>

#include <ophidian/standard_cell/StandardCells.h>

using namespace ophidian::standard_cell;

namespace {
class StandardCellsFixture {
public:
    std::string cell1Name, cell2Name;
    std::string pin1Name, pin2Name;
    PinDirection pin1Direction, pin2Direction;

    StandardCellsFixture()
        : cell1Name("cell1"), cell2Name("cell2"),
        pin1Name("pin1"), pin2Name("pin2"),
        pin1Direction(PinDirection::INPUT), pin2Direction(PinDirection::OUTPUT){

    }
};
}

TEST_CASE_METHOD(StandardCellsFixture, "Standard cells: creating cells", "[standard_cell]") {
    StandardCells stdCells;

    auto cell1 = stdCells.add(Cell(), cell1Name);
    auto cell2 = stdCells.add(Cell(), cell2Name);

    REQUIRE(stdCells.name(cell1) == cell1Name);
    REQUIRE(stdCells.name(cell2) == cell2Name);
    REQUIRE(stdCells.name(cell1) != stdCells.name(cell2));
}

TEST_CASE_METHOD(StandardCellsFixture, "Standard cells: creating pins", "[standard_cell]") {
    StandardCells stdCells;

    auto pin1 = stdCells.add(Pin(), pin1Name, pin1Direction);
    auto pin2 = stdCells.add(Pin(), pin2Name, pin2Direction);

    REQUIRE(stdCells.name(pin1) == pin1Name);
    REQUIRE(stdCells.name(pin2) == pin2Name);
    REQUIRE(stdCells.name(pin1) != stdCells.name(pin2));

    REQUIRE(stdCells.direction(pin1) == pin1Direction);
    REQUIRE(stdCells.direction(pin2) == pin2Direction);
    REQUIRE(stdCells.direction(pin1) != stdCells.direction(pin2));
}

TEST_CASE_METHOD(StandardCellsFixture, "Standard cells: associating pin to net", "[standard_cell]") {
    StandardCells stdCells;

    auto cell1 = stdCells.add(Cell(), cell1Name);
    auto pin1 = stdCells.add(Pin(), pin1Name, pin1Direction);

    stdCells.add(cell1, pin1);

    REQUIRE(stdCells.owner(pin1) == cell1);
    REQUIRE(stdCells.pins(cell1).size() == 1);
    REQUIRE(std::count(stdCells.pins(cell1).begin(), stdCells.pins(cell1).end(), pin1) == 1);
}
