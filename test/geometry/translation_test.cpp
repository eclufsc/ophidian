#include <catch.hpp>

#include <ophidian/geometry/Models.h>
#include <ophidian/geometry/Operations.h>

using namespace ophidian::geometry;

TEST_CASE("Geometry: translating a Point", "[geometry][operations]") {
    auto translationPoint = Point<double>{1.0, 1.0};

    auto point = Point<double>{1.0, 2.0};

    auto translatedPoint = translate(point, translationPoint);

    CHECK(translatedPoint.x() == Approx(2.0));
    CHECK(translatedPoint.y() == Approx(3.0));
}
//
// TEST_CASE("Geometry: translating a Box", "[geometry][operations]") {
//     Point translationPoint(1.0, 1.0);
//
//     BoxFixture boxFixture;
//
//     Box translatedBox = translate(boxFixture.box, translationPoint);
//
//     REQUIRE(translatedBox.min_corner().x() == boxFixture.xMin + translationPoint.x());
//     REQUIRE(translatedBox.min_corner().y() == boxFixture.yMin + translationPoint.y());
//     REQUIRE(translatedBox.max_corner().x() == boxFixture.xMax + translationPoint.x());
//     REQUIRE(translatedBox.max_corner().y() == boxFixture.yMax + translationPoint.y());
// }
