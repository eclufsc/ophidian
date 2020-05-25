#include <catch2/catch.hpp>

#include <ophidian/circuit/StandardCells.h>

using namespace ophidian::circuit;

namespace
{
class StandardCellsFixture
{
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

TEST_CASE_METHOD(StandardCellsFixture, "Standard cells: creating cells", "[standard_cell]")
{
	StandardCells stdCells;

	auto cell1 = stdCells.add_cell(cell1Name);
	auto cell2 = stdCells.add_cell(cell2Name);

	REQUIRE(stdCells.name(cell1) == cell1Name);
	REQUIRE(stdCells.name(cell2) == cell2Name);
	REQUIRE(stdCells.name(cell1) != stdCells.name(cell2));
}

TEST_CASE_METHOD(StandardCellsFixture, "Standard cells: try to add same cell twice", "[standard_cell]")
{
	StandardCells stdCells;

	auto cell1 = stdCells.add_cell(cell1Name);
	auto cell2 = stdCells.add_cell(cell1Name);

	REQUIRE(stdCells.name(cell1) == stdCells.name(cell2));
	REQUIRE (stdCells.size_cell() == 1);
}

TEST_CASE_METHOD(StandardCellsFixture, "Standard cells: cell mapping", "[standard_cell]")
{
	StandardCells stdCells;

	stdCells.add_cell(cell1Name);
	auto cell1 = stdCells.find_cell(cell1Name);
	stdCells.erase(cell1);
	REQUIRE (stdCells.size_cell() == 0);
}

TEST_CASE_METHOD(StandardCellsFixture, "Standard cells: creating pins", "[standard_cell]")
{
	StandardCells stdCells;

	auto pin1 = stdCells.add_pin(pin1Name, pin1Direction);
	auto pin2 = stdCells.add_pin(pin2Name, pin2Direction);

	REQUIRE(stdCells.name(pin1) == pin1Name);
	REQUIRE(stdCells.name(pin2) == pin2Name);
	REQUIRE(stdCells.name(pin1) != stdCells.name(pin2));

	REQUIRE(stdCells.direction(pin1) == pin1Direction);
	REQUIRE(stdCells.direction(pin2) == pin2Direction);
	REQUIRE(stdCells.direction(pin1) != stdCells.direction(pin2));
}

TEST_CASE_METHOD(StandardCellsFixture, "Standard cells: try to add same pin twice", "[standard_cell]")
{
	StandardCells stdCells;

	auto pin1 = stdCells.add_pin(pin1Name, pin1Direction);
	auto pin2 = stdCells.add_pin(pin1Name, pin1Direction);

	REQUIRE(stdCells.name(pin1) == stdCells.name(pin2));
	REQUIRE(stdCells.size_pin() == 1);
}

TEST_CASE_METHOD(StandardCellsFixture, "Standard cells: pin mapping", "[standard_cell]")
{
	StandardCells stdCells;

	stdCells.add_pin(pin1Name, pin1Direction);
	auto pin1 = stdCells.find_pin(pin1Name);
	stdCells.erase(pin1);

	REQUIRE(stdCells.size_pin() == 0);
}

TEST_CASE_METHOD(StandardCellsFixture, "Standard cells: associating pin to net", "[standard_cell]")
{
	StandardCells stdCells;

	auto cell1 = stdCells.add_cell(cell1Name);
	auto pin1 = stdCells.add_pin(pin1Name, pin1Direction);

	stdCells.connect(cell1, pin1);

	REQUIRE(stdCells.cell(pin1) == cell1);
	REQUIRE(stdCells.pins(cell1).size() == 1);
	REQUIRE(std::count(stdCells.pins(cell1).begin(), stdCells.pins(cell1).end(), pin1) == 1);
}
