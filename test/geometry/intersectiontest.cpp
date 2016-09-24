#include <catch.hpp>

#include <ophidian/geometry/Operations.h>

using namespace ophidian::geometry;

TEST_CASE("Geometry: intersection between two boxes", "[geometry][operations]") {
    Box box1(Point(1.0, 1.0), Point(3.0, 3.0));
    Box box2(Point(2.0, 1.0), Point(4.0, 3.0));

    Box intersectionBox;
    intersection(box1, box2, intersectionBox);

    REQUIRE(intersectionBox.min_corner().x() == Approx(2.0));
    REQUIRE(intersectionBox.min_corner().y() == Approx(1.0));
    REQUIRE(intersectionBox.max_corner().x() == Approx(3.0));
    REQUIRE(intersectionBox.max_corner().y() == Approx(3.0));
}

TEST_CASE("Geometry: intersection between two polygons", "[geometry][operations]") {
    std::vector<Point> points1 = {
        Point(1.0, 1.0),
        Point(1.0, 3.0),
        Point(3.0, 3.0),
        Point(3.0, 1.0),
        Point(1.0, 1.0),
    };
    Polygon polygon1 = make<Polygon>(points1);

    std::vector<Point> points2 = {
        Point(2.0, 1.0),
        Point(2.0, 3.0),
        Point(4.0, 3.0),
        Point(4.0, 1.0),
        Point(2.0, 1.0),
    };
    Polygon polygon2 = make<Polygon>(points2);

    MultiPolygon intersection;
    //intersection(polygon1, polygon2, intersection);
    boost::geometry::intersection(polygon1, polygon2, intersection);

    REQUIRE(intersection.size() == 1);

    std::vector<Point> intersectionPoints(intersection.at(0).outer().begin(), intersection.at(0).outer().end());
    REQUIRE(intersectionPoints.at(0).x() == Approx(2.0));
    REQUIRE(intersectionPoints.at(0).y() == Approx(3.0));
    REQUIRE(intersectionPoints.at(1).x() == Approx(3.0));
    REQUIRE(intersectionPoints.at(1).y() == Approx(3.0));
    REQUIRE(intersectionPoints.at(2).x() == Approx(3.0));
    REQUIRE(intersectionPoints.at(2).y() == Approx(1.0));
    REQUIRE(intersectionPoints.at(3).x() == Approx(2.0));
    REQUIRE(intersectionPoints.at(3).y() == Approx(1.0));
    REQUIRE(intersectionPoints.at(4).x() == Approx(2.0));
    REQUIRE(intersectionPoints.at(4).y() == Approx(3.0));
}
