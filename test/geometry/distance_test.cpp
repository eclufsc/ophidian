#include <catch.hpp>

#include <ophidian/geometry/Distance.h>

using namespace ophidian::geometry;

TEST_CASE("Geometry: manhattan distance between two points", "[geometry][distance]") {
    Point<double> point1(5, 10);
    Point<double> point2(3, 7);
    double distance = ManhattanDistance(point1, point2);
    double expected_distance = 5;
    REQUIRE(distance == Approx(expected_distance));
}

TEST_CASE("Geometry: euclidean distance between two points", "[geometry][distance]") {
    Point<double> point1(5, 10);
    Point<double> point2(3, 7);
    double distance = EuclideanDistance(point1, point2);
    double expected_distance = std::sqrt(13);
    REQUIRE(distance == Approx(expected_distance));
}
