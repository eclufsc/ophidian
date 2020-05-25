#include <catch2/catch.hpp>

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

TEST_CASE("Geometry: translating a Box", "[geometry][operations]") {
    auto translationPoint = Point<double>{1.0, 1.0};

    auto point = Box<double>{
        {1.0, 2.0},
        {2.0, 2.0}
    };

    auto translatedBox = translate(point, translationPoint);

    REQUIRE(translatedBox.min_corner().x() == Approx(2.0));
    REQUIRE(translatedBox.min_corner().y() == Approx(3.0));
    REQUIRE(translatedBox.max_corner().x() == Approx(3.0));
    REQUIRE(translatedBox.max_corner().y() == Approx(3.0));
}
