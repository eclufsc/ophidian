#include <catch.hpp>

#include <ophidian/geometry/Models.h>

#include "modelsfixture.h"

using namespace ophidian::geometry;

// TEST_CASE("Geometry: creating new Point", "[geometry][models]") {
//     PointFixture pointFixture;
//     Point point = pointFixture.point;
//     REQUIRE(point.x() == pointFixture.x);
//     REQUIRE(point.y() == pointFixture.y);
// }
//
// TEST_CASE("Geometry: creating new Segment", "[geometry][models]") {
//     SegmentFixture segmentFixture;
//     Segment segment = segmentFixture.segment;
//     REQUIRE(segment.first.x() == segmentFixture.x1);
//     REQUIRE(segment.first.y() == segmentFixture.y1);
//     REQUIRE(segment.second.x() == segmentFixture.x2);
//     REQUIRE(segment.second.y() == segmentFixture.y2);
// }
//
// TEST_CASE("Geometry: creating new Linestring", "[geometry][models]") {
//     LinestringFixture linestringFixture;
//     Linestring linestring = linestringFixture.linestring;
//     std::vector<Point> linestringPoints(linestring.begin(), linestring.end());
//     REQUIRE(linestringPoints.size() == 3);
//     REQUIRE(linestringPoints.at(0).x() == linestringFixture.x1);
//     REQUIRE(linestringPoints.at(0).y() == linestringFixture.y1);
//     REQUIRE(linestringPoints.at(1).x() == linestringFixture.x2);
//     REQUIRE(linestringPoints.at(1).y() == linestringFixture.y2);
//     REQUIRE(linestringPoints.at(2).x() == linestringFixture.x3);
//     REQUIRE(linestringPoints.at(2).y() == linestringFixture.y3);
// }
//
// TEST_CASE("Geometry: creating new Box", "[geometry][models]") {
//     BoxFixture boxFixture;
//     Box box = boxFixture.box;
//     REQUIRE(box.min_corner().x() == boxFixture.xMin);
//     REQUIRE(box.min_corner().y() == boxFixture.yMin);
//     REQUIRE(box.max_corner().x() == boxFixture.xMax);
//     REQUIRE(box.max_corner().y() == boxFixture.yMax);
// }
//
// TEST_CASE("Geometry: creating new Polygon", "[geometry][models]") {
//     PolygonFixture polygonFixture;
//     Polygon polygon = polygonFixture.polygon;
//     std::vector<Point> polygonPoints(polygon.outer().begin(), polygon.outer().end());
//     REQUIRE(polygonPoints.size() == 5);
//     REQUIRE(polygonPoints.at(0).x() == polygonFixture.x1);
//     REQUIRE(polygonPoints.at(0).y() == polygonFixture.y1);
//     REQUIRE(polygonPoints.at(1).x() == polygonFixture.x2);
//     REQUIRE(polygonPoints.at(1).y() == polygonFixture.y2);
//     REQUIRE(polygonPoints.at(2).x() == polygonFixture.x3);
//     REQUIRE(polygonPoints.at(2).y() == polygonFixture.y3);
//     REQUIRE(polygonPoints.at(3).x() == polygonFixture.x4);
//     REQUIRE(polygonPoints.at(3).y() == polygonFixture.y4);
//     REQUIRE(polygonPoints.at(4).x() == polygonFixture.x1);
//     REQUIRE(polygonPoints.at(4).y() == polygonFixture.y1);
// }
//
// TEST_CASE("Geometry: creating new MultiPolygon", "[geometry][models]") {
//     MultiPolygonFixture multiPolygonFixture;
//     MultiPolygon multiPolygon = multiPolygonFixture.multiPolygon;
//
//     std::vector<Point> polygon1Points(multiPolygon.at(0).outer().begin(), multiPolygon.at(0).outer().end());
//     REQUIRE(polygon1Points.size() == 5);
//     REQUIRE(polygon1Points.at(0).x() == multiPolygonFixture.x1);
//     REQUIRE(polygon1Points.at(0).y() == multiPolygonFixture.y1);
//     REQUIRE(polygon1Points.at(1).x() == multiPolygonFixture.x2);
//     REQUIRE(polygon1Points.at(1).y() == multiPolygonFixture.y2);
//     REQUIRE(polygon1Points.at(2).x() == multiPolygonFixture.x3);
//     REQUIRE(polygon1Points.at(2).y() == multiPolygonFixture.y3);
//     REQUIRE(polygon1Points.at(3).x() == multiPolygonFixture.x4);
//     REQUIRE(polygon1Points.at(3).y() == multiPolygonFixture.y4);
//     REQUIRE(polygon1Points.at(4).x() == multiPolygonFixture.x1);
//     REQUIRE(polygon1Points.at(4).y() == multiPolygonFixture.y1);
//
//     std::vector<Point> polygon2Points(multiPolygon.at(1).outer().begin(), multiPolygon.at(1).outer().end());
//     REQUIRE(polygon2Points.size() == 5);
//     REQUIRE(polygon2Points.at(0).x() == multiPolygonFixture.x5);
//     REQUIRE(polygon2Points.at(0).y() == multiPolygonFixture.y5);
//     REQUIRE(polygon2Points.at(1).x() == multiPolygonFixture.x6);
//     REQUIRE(polygon2Points.at(1).y() == multiPolygonFixture.y6);
//     REQUIRE(polygon2Points.at(2).x() == multiPolygonFixture.x7);
//     REQUIRE(polygon2Points.at(2).y() == multiPolygonFixture.y7);
//     REQUIRE(polygon2Points.at(3).x() == multiPolygonFixture.x8);
//     REQUIRE(polygon2Points.at(3).y() == multiPolygonFixture.y8);
//     REQUIRE(polygon2Points.at(4).x() == multiPolygonFixture.x5);
//     REQUIRE(polygon2Points.at(4).y() == multiPolygonFixture.y5);
// }
