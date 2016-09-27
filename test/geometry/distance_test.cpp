#include <catch.hpp>

#include <ophidian/geometry/Distance.h>

using namespace ophidian::geometry;

TEST_CASE("Geometry: manhattan distance between two points", "[geometry][distance]") {
    Point point1(5, 10);
    Point point2(3, 7);
    ManhattanDistance manhattanDistance;
    double distance = manhattanDistance(point1, point2);
    double expected_distance = 5;
    REQUIRE(distance == Approx(expected_distance));
}

TEST_CASE("Geometry: euclidean distance between two points", "[geometry][distance]") {
    Point point1(5, 10);
    Point point2(3, 7);
    EuclideanDistance euclideanDistance;
    double distance = euclideanDistance(point1, point2);
    double expected_distance = std::sqrt(13);
    REQUIRE(distance == Approx(expected_distance));
}
