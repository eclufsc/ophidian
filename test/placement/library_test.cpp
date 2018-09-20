#include <catch.hpp>

#include <ophidian/geometry/Models.h>
#include <ophidian/placement/Library.h>

using namespace ophidian::placement;
using namespace ophidian::circuit;
using namespace ophidian::geometry;

class StandardCellsFixture
{
public:
	Cell cell1, cell2;
	Pin pin1, pin2;

	StandardCells std_cells;

	StandardCellsFixture() {
        cell1 = std_cells.add_cell("cell1");
        cell2 = std_cells.add_cell("cell2");

		pin1 = std_cells.add_pin("pin1", PinDirection::NA);
		pin2 = std_cells.add_pin("pin2", PinDirection::NA);
	}
};

TEST_CASE_METHOD(StandardCellsFixture, "Library: setting geometry of cells", "[placement][library]")
{
    auto library = Library{std_cells};

    auto cell1Boxes = CellGeometry::box_container_type{
        {
            {CellGeometry::unit_type{0}, CellGeometry::unit_type{0}},
            {CellGeometry::unit_type{10}, CellGeometry::unit_type{10}}
        }
    };

    auto cell1Geometry = CellGeometry{std::move(cell1Boxes)};

    library.geometry(cell1) = cell1Geometry;

    auto cell2Boxes = CellGeometry::box_container_type{
        {
            {CellGeometry::unit_type{0}, CellGeometry::unit_type{0}},
            {CellGeometry::unit_type{20}, CellGeometry::unit_type{10}}
        }
    };

    auto cell2Geometry = CellGeometry{std::move(cell2Boxes)};

    library.geometry(cell2) = cell2Geometry;

    REQUIRE(cell1Geometry == library.geometry(cell1));
    REQUIRE(cell2Geometry == library.geometry(cell2));
    REQUIRE(library.geometry(cell1) != library.geometry(cell2));
}

TEST_CASE_METHOD(StandardCellsFixture, "Library: setting offset of pins", "[placement][library]")
{
    auto library = Library{std_cells};

    auto pin1Offset = Library::offset_type{
        Library::unit_type{5},
        Library::unit_type{5}
    };

    library.offset(pin1) = pin1Offset;

    auto pin2Offset = Library::offset_type{
        Library::unit_type{3},
        Library::unit_type{3}
    };

    library.offset(pin2) = pin2Offset;

    REQUIRE(pin1Offset.x() == library.offset(pin1).x());
    REQUIRE(pin1Offset.y() == library.offset(pin1).y());
    REQUIRE(pin2Offset.x() == library.offset(pin2).x());
    REQUIRE(pin2Offset.y() == library.offset(pin2).y());
    REQUIRE(library.offset(pin1).x() != library.offset(pin2).x());
    REQUIRE(library.offset(pin1).y() != library.offset(pin2).y());
}
