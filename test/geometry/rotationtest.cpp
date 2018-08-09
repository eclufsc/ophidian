#include <catch.hpp>

#include <ophidian/geometry/Operations.h>

#include "modelsfixture.h"

using namespace ophidian::geometry;

// TEST_CASE("Geometry: rotating a Point", "[geometry][operations]") {
//     double degree = 90;
//
//     PointFixture pointFixture;
//
//     Point rotatedPoint;
//     rotate(pointFixture.point, degree, rotatedPoint);
//
//     REQUIRE(rotatedPoint.x() == Approx(2.0));
//     REQUIRE(rotatedPoint.y() == Approx(-1.0));
// }
//
// TEST_CASE("Geometry: rotating a Segment", "[geometry][operations]") {
//     double degree = 90;
//
//     SegmentFixture segmentFixture;
//
//     Segment rotatedSegment;
//     rotate(segmentFixture.segment, degree, rotatedSegment);
//
//     REQUIRE(rotatedSegment.first.x() == Approx(2.0));
//     REQUIRE(rotatedSegment.first.y() == Approx(-1.0));
//     REQUIRE(rotatedSegment.second.x() == Approx(1.0));
//     REQUIRE(rotatedSegment.second.y() == Approx(-2.0));
// }
//
// TEST_CASE("Geometry: rotating a Linestring", "[geometry][operations]") {
//     double degree = 90;
//
//     LinestringFixture linestringFixture;
//
//     Linestring rotatedLinestring;
//     rotate(linestringFixture.linestring, degree, rotatedLinestring);
//
//     std::vector<Point> linestringPoints(rotatedLinestring.begin(), rotatedLinestring.end());
//     REQUIRE(linestringPoints.size() == 3);
//     REQUIRE(linestringPoints.at(0).x() == Approx(2.0));
//     REQUIRE(linestringPoints.at(0).y() == Approx(-1.0));
//     REQUIRE(linestringPoints.at(1).x() == Approx(1.0));
//     REQUIRE(linestringPoints.at(1).y() == Approx(-2.0));
//     REQUIRE(linestringPoints.at(2).x() == Approx(2.0));
//     REQUIRE(linestringPoints.at(2).y() == Approx(-2.0));
// }
//
// TEST_CASE("Geometry: rotating a Box", "[geometry][operations]") {
//     double degree = 90;
//
//     BoxFixture boxFixture;
//
//     Box rotatedBox;
//     rotate(boxFixture.box, degree, rotatedBox);
//
//     REQUIRE(rotatedBox.min_corner().x() == Approx(2.0));
//     REQUIRE(rotatedBox.min_corner().y() == Approx(-2.0));
//     REQUIRE(rotatedBox.max_corner().x() == Approx(2.0));
//     REQUIRE(rotatedBox.max_corner().y() == Approx(-1.0));
// }
//
// TEST_CASE("Geometry: rotating a Polygon", "[geometry][operations]") {
//     double degree = 90;
//
//     PolygonFixture polygonFixture;
//
//     Polygon rotatedPolygon;
//     rotate(polygonFixture.polygon, degree, rotatedPolygon);
//
//     std::vector<Point> polygonPoints(rotatedPolygon.outer().begin(), rotatedPolygon.outer().end());
//     REQUIRE(polygonPoints.size() == 5);
//     REQUIRE(polygonPoints.at(0).x() == Approx(1.0));
//     REQUIRE(polygonPoints.at(0).y() == Approx(-1.0));
//     REQUIRE(polygonPoints.at(1).x() == Approx(1.0));
//     REQUIRE(polygonPoints.at(1).y() == Approx(-2.0));
//     REQUIRE(polygonPoints.at(2).x() == Approx(2.0));
//     REQUIRE(polygonPoints.at(2).y() == Approx(-2.0));
//     REQUIRE(polygonPoints.at(3).x() == Approx(2.0));
//     REQUIRE(polygonPoints.at(3).y() == Approx(-1.0));
//     REQUIRE(polygonPoints.at(4).x() == Approx(1.0));
//     REQUIRE(polygonPoints.at(4).y() == Approx(-1.0));
// }
//
// TEST_CASE("Geometry: rotating a MultiPolygon", "[geometry][operations]") {
//     double degree = 90;
//
//     MultiPolygonFixture multiPolygonFixture;
//
//     MultiPolygon rotatedMultiPolygon;
//     rotate(multiPolygonFixture.multiPolygon, degree, rotatedMultiPolygon);
//
//     std::vector<Point> polygon1Points(rotatedMultiPolygon.at(0).outer().begin(), rotatedMultiPolygon.at(0).outer().end());
//     REQUIRE(polygon1Points.size() == 5);
//     REQUIRE(polygon1Points.at(0).x() == Approx(1.0));
//     REQUIRE(polygon1Points.at(0).y() == Approx(-1.0));
//     REQUIRE(polygon1Points.at(1).x() == Approx(1.0));
//     REQUIRE(polygon1Points.at(1).y() == Approx(-2.0));
//     REQUIRE(polygon1Points.at(2).x() == Approx(2.0));
//     REQUIRE(polygon1Points.at(2).y() == Approx(-2.0));
//     REQUIRE(polygon1Points.at(3).x() == Approx(2.0));
//     REQUIRE(polygon1Points.at(3).y() == Approx(-1.0));
//     REQUIRE(polygon1Points.at(4).x() == Approx(1.0));
//     REQUIRE(polygon1Points.at(4).y() == Approx(-1.0));
//
//     std::vector<Point> polygon2Points(rotatedMultiPolygon.at(1).outer().begin(), rotatedMultiPolygon.at(1).outer().end());
//     REQUIRE(polygon2Points.size() == 5);
//     REQUIRE(polygon2Points.at(0).x() == Approx(1.0));
//     REQUIRE(polygon2Points.at(0).y() == Approx(-3.0));
//     REQUIRE(polygon2Points.at(1).x() == Approx(1.0));
//     REQUIRE(polygon2Points.at(1).y() == Approx(-4.0));
//     REQUIRE(polygon2Points.at(2).x() == Approx(2.0));
//     REQUIRE(polygon2Points.at(2).y() == Approx(-4.0));
//     REQUIRE(polygon2Points.at(3).x() == Approx(2.0));
//     REQUIRE(polygon2Points.at(3).y() == Approx(-3.0));
//     REQUIRE(polygon2Points.at(4).x() == Approx(1.0));
//     REQUIRE(polygon2Points.at(4).y() == Approx(-3.0));
// }
