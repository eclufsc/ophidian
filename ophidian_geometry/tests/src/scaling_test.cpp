#include <catch2/catch.hpp>

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

TEST_CASE("Geometry: scaling a Box", "[geometry][operations]") {
    auto scalingPoint = Point<double>{2.0, 2.0};

    auto box = Box<double>{{1.0, 2.0}, {2.0, 2.0}};

    auto scaledBox = scale(box, scalingPoint);

    CHECK(scaledBox.min_corner().x() == Approx(2.0));
    CHECK(scaledBox.min_corner().y() == Approx(4.0));
    CHECK(scaledBox.max_corner().x() == Approx(4.0));
    CHECK(scaledBox.max_corner().y() == Approx(4.0));
}
