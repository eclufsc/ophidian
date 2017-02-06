#include <catch.hpp>

#include <boost/geometry.hpp>

#include <ophidian/placement/Library.h>

using namespace ophidian::placement;
using namespace ophidian::standard_cell;
using namespace ophidian::geometry;

namespace {
class StandardCellsFixture {
public:
    Cell cell1, cell2;
    Pin pin1, pin2;

    StandardCells std_cells;

    StandardCellsFixture() {
        cell1 = std_cells.add(Cell());
        cell2 = std_cells.add(Cell());

        pin1 = std_cells.add(Pin());
        pin2 = std_cells.add(Pin());
    }
};
}

TEST_CASE_METHOD(StandardCells, "Library: setting geometry of cells", "[placement][library]") {
    Library library(std_cells);

    MultiBox cell1Geometry = {Box(Point(0, 0), Point(10, 10))};
    library.geometry(cell1, cell1Geometry);

    MultiBox cell2Geometry = {Box(Point(0, 0), Point(20, 10))};
    library.geometry(cell2, cell2Geometry);

    REQUIRE(boost::geometry::equals(cell1Geometry, library.geometry(cell1)));
    REQUIRE(boost::geometry::equals(cell2Geometry, library.geometry(cell2)));
    REQUIRE(boost::geometry::equals(library.geometry(cell1), library.geometry(cell2)));
}

TEST_CASE_METHOD(StandardCells, "Library: setting offset of pins", "[placement][library]") {
    Library library(std_cells);

    Point pin1Offset(5, 5);
    library.pinOffset(pin1, pin1Offset);

    Point pin2Offset(3, 3);
    library.pinOffset(pin2, pin2Offset);

    REQUIRE(boost::geometry::equals(pin1Offset, library.pinOffset(pin1)));
    REQUIRE(boost::geometry::equals(pin2Offset, library.pinOffset(pin2)));
    REQUIRE(boost::geometry::equals(library.pinOffset(pin1), library.pinOffset(pin2)));
}
