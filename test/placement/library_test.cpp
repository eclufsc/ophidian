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

bool multiBoxComparison(const MultiBox & multiBox1, const MultiBox & multiBox2) {
    for (auto box1 : multiBox1) {
        for (auto box2 : multiBox2) {
            bool comparison = (box1.min_corner().x() == box2.min_corner().x()) && (box1.min_corner().y() == box2.min_corner().y())
                    && (box1.max_corner().x() == box2.max_corner().x()) && (box1.max_corner().y() == box2.max_corner().y());
            if (!comparison) {
                return false;
            }
        }
    }
    return true;
}
}


TEST_CASE_METHOD(StandardCellsFixture, "Library: setting geometry of cells", "[placement][library]") {
    Library library(std_cells);

    MultiBox cell1Geometry = {Box(Point(0, 0), Point(10, 10))};
    library.geometry(cell1, cell1Geometry);

    MultiBox cell2Geometry = {Box(Point(0, 0), Point(20, 10))};
    library.geometry(cell2, cell2Geometry);

    REQUIRE(multiBoxComparison(cell1Geometry, library.geometry(cell1)));
    REQUIRE(multiBoxComparison(cell2Geometry, library.geometry(cell2)));
    REQUIRE(!multiBoxComparison(library.geometry(cell1), library.geometry(cell2)));
}

TEST_CASE_METHOD(StandardCellsFixture, "Library: setting offset of pins", "[placement][library]") {
    Library library(std_cells);

    ophidian::util::Location pin1Offset(5, 5);
    library.pinOffset(pin1, pin1Offset);

    ophidian::util::Location pin2Offset(3, 3);
    library.pinOffset(pin2, pin2Offset);

    REQUIRE(pin1Offset == library.pinOffset(pin1));
    REQUIRE(pin2Offset == library.pinOffset(pin2));
    REQUIRE(!(library.pinOffset(pin1) == library.pinOffset(pin2)));
}
