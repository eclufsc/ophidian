#include <catch.hpp>

#include <ophidian/geometry/Models.h>
#include <ophidian/placement/Library.h>

using namespace ophidian::placement;
using namespace ophidian::standard_cell;
using namespace ophidian::geometry;

namespace
{
class StandardCellsFixture
{
public:
	Cell cell1, cell2;
	Pin pin1, pin2;

	StandardCells std_cells;

	StandardCellsFixture() {
		cell1 = std_cells.add(ophidian::standard_cell::Cell(), "cell1");
		cell2 = std_cells.add(ophidian::standard_cell::Cell(), "cell2");

		pin1 = std_cells.add(Pin(), "pin1", PinDirection::NA);
		pin2 = std_cells.add(Pin(), "pin2", PinDirection::NA);
	}
};

} // namespace
  //


// TEST_CASE_METHOD(StandardCellsFixture, "Library: setting geometry of cells", "[placement][library]")
// {
//     Library library(std_cells);
//
//     std::vector<Box> cell1Boxes = {Box(Point(0, 0), Point(10, 10))};
//     MultiBox cell1Geometry(cell1Boxes);
//     library.geometry(cell1, cell1Geometry);
//
//     std::vector<Box> cell2Boxes = {Box(Point(0, 0), Point(20, 10))};
//     MultiBox cell2Geometry(cell2Boxes);
//     library.geometry(cell2, cell2Geometry);
//
//     REQUIRE(cell1Geometry == library.geometry(cell1));
//     REQUIRE(cell2Geometry == library.geometry(cell2));
//     REQUIRE(library.geometry(cell1) != library.geometry(cell2));
// }

// TEST_CASE_METHOD(StandardCellsFixture, "Library: setting offset of pins", "[placement][library]")
// {
//     Library library(std_cells);
//
//     ophidian::util::LocationDbu pin1Offset(5, 5);
//     library.pinOffset(pin1, pin1Offset);
//
//     ophidian::util::LocationDbu pin2Offset(3, 3);
//     library.pinOffset(pin2, pin2Offset);
//
//     REQUIRE(pin1Offset == library.pinOffset(pin1));
//     REQUIRE(pin2Offset == library.pinOffset(pin2));
//     REQUIRE(library.pinOffset(pin1) != library.pinOffset(pin2));
// }
