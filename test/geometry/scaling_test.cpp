#include <catch.hpp>

#include <ophidian/geometry/Models.h>
#include <ophidian/geometry/Operations.h>

using namespace ophidian::geometry;

TEST_CASE("Geometry: scaling a Point", "[geometry][operations]")
{
    auto scalingPoint = Point<double>{2.0, 2.0};

    auto point = Point<double>{1.0, 2.0};

    auto scaledPoint = scale(point, scalingPoint);

    CHECK(scaledPoint.x() == Approx(2.0));
    CHECK(scaledPoint.y() == Approx(4.0));
}
//
// TEST_CASE("Geometry: scaling a Box", "[geometry][operations]") {
//     Point scalingPoint(2.0, 2.0);
//
//     BoxFixture boxFixture;
//
//     Box scaledBox = scale(boxFixture.box, scalingPoint);
//
//     REQUIRE(scaledBox.min_corner().x() == boxFixture.xMin * scalingPoint.x());
//     REQUIRE(scaledBox.min_corner().y() == boxFixture.yMin * scalingPoint.y());
//     REQUIRE(scaledBox.max_corner().x() == boxFixture.xMax * scalingPoint.x());
//     REQUIRE(scaledBox.max_corner().y() == boxFixture.yMax * scalingPoint.y());
// }
