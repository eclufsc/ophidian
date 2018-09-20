#include <catch.hpp>

#include <ophidian/geometry/Operations.h>

using namespace ophidian::geometry;

TEST_CASE("Geometry: intersection between two boxes", "[geometry][operations]") {
    Box<double> box1({1.0, 1.0}, {3.0, 3.0});
    Box<double> box2({2.0, 1.0}, {4.0, 3.0});

    Box<double> intersectionBox;
    intersection(box1, box2, intersectionBox);

    REQUIRE(intersectionBox.min_corner().x() == Approx(2.0));
    REQUIRE(intersectionBox.min_corner().y() == Approx(1.0));
    REQUIRE(intersectionBox.max_corner().x() == Approx(3.0));
    REQUIRE(intersectionBox.max_corner().y() == Approx(3.0));
}

// TEST_CASE("Geometry: intersection between two polygons", "[geometry][operations]") {
//     std::vector<Point<double>> points1 = {
//         Point<double>{1.0, 1.0},
//         Point<double>{1.0, 3.0},
//         Point<double>{3.0, 3.0},
//         Point<double>{3.0, 1.0},
//         Point<double>{1.0, 1.0},
//     };
//     auto polygon1 = Polygon<double>{points1};
//
//     std::vector<Point<double>> points2 = {
//         Point<double>{2.0, 1.0},
//         Point<double>{2.0, 3.0},
//         Point<double>{4.0, 3.0},
//         Point<double>{4.0, 1.0},
//         Point<double>{2.0, 1.0},
//     };
//     auto polygon2 = Polygon<double>{points2};
//
//     MultiPolygon<double> intersectionResult;
//     intersection(polygon1, polygon2, intersectionResult);
//
//     REQUIRE(intersectionResult.size() == 1);
//
//     std::vector<Point<double>> intersectionPoints(intersectionResult.at(0).outer().begin(), intersectionResult.at(0).outer().end());
//     REQUIRE(intersectionPoints.at(0).x() == Approx(2.0));
//     REQUIRE(intersectionPoints.at(0).y() == Approx(3.0));
//     REQUIRE(intersectionPoints.at(1).x() == Approx(3.0));
//     REQUIRE(intersectionPoints.at(1).y() == Approx(3.0));
//     REQUIRE(intersectionPoints.at(2).x() == Approx(3.0));
//     REQUIRE(intersectionPoints.at(2).y() == Approx(1.0));
//     REQUIRE(intersectionPoints.at(3).x() == Approx(2.0));
//     REQUIRE(intersectionPoints.at(3).y() == Approx(1.0));
//     REQUIRE(intersectionPoints.at(4).x() == Approx(2.0));
//     REQUIRE(intersectionPoints.at(4).y() == Approx(3.0));
// }
