#include <catch.hpp>

#include <ophidian/geometry/Operations.h>

#include "modelsfixture.h"

using namespace ophidian::geometry;

TEST_CASE("Geometry: translating a Point", "[geometry][operations]") {
    Point translationPoint(1.0, 1.0);

    PointFixture pointFixture;

    Point translatedPoint;
    translate(pointFixture.point, translationPoint, translatedPoint);

    REQUIRE(translatedPoint.x() == pointFixture.x + translationPoint.x());
    REQUIRE(translatedPoint.y() == pointFixture.y + translationPoint.y());
}

TEST_CASE("Geometry: translating a Segment", "[geometry][operations]") {
    Point translationPoint(1.0, 1.0);

    SegmentFixture segmentFixture;

    Segment translatedSegment;
    translate(segmentFixture.segment, translationPoint, translatedSegment);

    REQUIRE(translatedSegment.first.x() == segmentFixture.x1 + translationPoint.x());
    REQUIRE(translatedSegment.first.y() == segmentFixture.y1 + translationPoint.y());
    REQUIRE(translatedSegment.second.x() == segmentFixture.x2 + translationPoint.x());
    REQUIRE(translatedSegment.second.y() == segmentFixture.y2 + translationPoint.y());
}

TEST_CASE("Geometry: translating a Linestring", "[geometry][operations]") {
    Point translationPoint(1.0, 1.0);

    LinestringFixture linestringFixture;

    Linestring translatedLinestring;
    translate(linestringFixture.linestring, translationPoint, translatedLinestring);

    std::vector<Point> linestringPoints(translatedLinestring.begin(), translatedLinestring.end());
    REQUIRE(linestringPoints.size() == 3);
    REQUIRE(linestringPoints.at(0).x() == linestringFixture.x1 + translationPoint.x());
    REQUIRE(linestringPoints.at(0).y() == linestringFixture.y1 + translationPoint.y());
    REQUIRE(linestringPoints.at(1).x() == linestringFixture.x2 + translationPoint.x());
    REQUIRE(linestringPoints.at(1).y() == linestringFixture.y2 + translationPoint.y());
    REQUIRE(linestringPoints.at(2).x() == linestringFixture.x3 + translationPoint.x());
    REQUIRE(linestringPoints.at(2).y() == linestringFixture.y3 + translationPoint.y());
}

TEST_CASE("Geometry: translating a Box", "[geometry][operations]") {
    Point translationPoint(1.0, 1.0);

    BoxFixture boxFixture;

    Box translatedBox;
    translate(boxFixture.box, translationPoint, translatedBox);

    REQUIRE(translatedBox.min_corner().x() == boxFixture.xMin + translationPoint.x());
    REQUIRE(translatedBox.min_corner().y() == boxFixture.yMin + translationPoint.y());
    REQUIRE(translatedBox.max_corner().x() == boxFixture.xMax + translationPoint.x());
    REQUIRE(translatedBox.max_corner().y() == boxFixture.yMax + translationPoint.y());
}

TEST_CASE("Geometry: translating a Polygon", "[geometry][operations]") {
    Point translationPoint(1.0, 1.0);

    PolygonFixture polygonFixture;

    Polygon translatedPolygon;
    translate(polygonFixture.polygon, translationPoint, translatedPolygon);

    std::vector<Point> polygonPoints(translatedPolygon.outer().begin(), translatedPolygon.outer().end());
    REQUIRE(polygonPoints.size() == 5);
    REQUIRE(polygonPoints.at(0).x() == polygonFixture.x1 + translationPoint.x());
    REQUIRE(polygonPoints.at(0).y() == polygonFixture.y1 + translationPoint.y());
    REQUIRE(polygonPoints.at(1).x() == polygonFixture.x2 + translationPoint.x());
    REQUIRE(polygonPoints.at(1).y() == polygonFixture.y2 + translationPoint.y());
    REQUIRE(polygonPoints.at(2).x() == polygonFixture.x3 + translationPoint.x());
    REQUIRE(polygonPoints.at(2).y() == polygonFixture.y3 + translationPoint.y());
    REQUIRE(polygonPoints.at(3).x() == polygonFixture.x4 + translationPoint.x());
    REQUIRE(polygonPoints.at(3).y() == polygonFixture.y4 + translationPoint.y());
    REQUIRE(polygonPoints.at(4).x() == polygonFixture.x1 + translationPoint.x());
    REQUIRE(polygonPoints.at(4).y() == polygonFixture.y1 + translationPoint.y());
}

TEST_CASE("Geometry: translating a MultiPolygon", "[geometry][operations]") {
    Point translationPoint(1.0, 1.0);

    MultiPolygonFixture multiPolygonFixture;

    MultiPolygon translatedMultiPolygon;
    translate(multiPolygonFixture.multiPolygon, translationPoint, translatedMultiPolygon);

    std::vector<Point> polygon1Points(translatedMultiPolygon.at(0).outer().begin(), translatedMultiPolygon.at(0).outer().end());
    REQUIRE(polygon1Points.size() == 5);
    REQUIRE(polygon1Points.at(0).x() == multiPolygonFixture.x1 + translationPoint.x());
    REQUIRE(polygon1Points.at(0).y() == multiPolygonFixture.y1 + translationPoint.y());
    REQUIRE(polygon1Points.at(1).x() == multiPolygonFixture.x2 + translationPoint.x());
    REQUIRE(polygon1Points.at(1).y() == multiPolygonFixture.y2 + translationPoint.y());
    REQUIRE(polygon1Points.at(2).x() == multiPolygonFixture.x3 + translationPoint.x());
    REQUIRE(polygon1Points.at(2).y() == multiPolygonFixture.y3 + translationPoint.y());
    REQUIRE(polygon1Points.at(3).x() == multiPolygonFixture.x4 + translationPoint.x());
    REQUIRE(polygon1Points.at(3).y() == multiPolygonFixture.y4 + translationPoint.y());
    REQUIRE(polygon1Points.at(4).x() == multiPolygonFixture.x1 + translationPoint.x());
    REQUIRE(polygon1Points.at(4).y() == multiPolygonFixture.y1 + translationPoint.y());

    std::vector<Point> polygon2Points(translatedMultiPolygon.at(1).outer().begin(), translatedMultiPolygon.at(1).outer().end());
    REQUIRE(polygon2Points.size() == 5);
    REQUIRE(polygon2Points.at(0).x() == multiPolygonFixture.x5 + translationPoint.x());
    REQUIRE(polygon2Points.at(0).y() == multiPolygonFixture.y5 + translationPoint.y());
    REQUIRE(polygon2Points.at(1).x() == multiPolygonFixture.x6 + translationPoint.x());
    REQUIRE(polygon2Points.at(1).y() == multiPolygonFixture.y6 + translationPoint.y());
    REQUIRE(polygon2Points.at(2).x() == multiPolygonFixture.x7 + translationPoint.x());
    REQUIRE(polygon2Points.at(2).y() == multiPolygonFixture.y7 + translationPoint.y());
    REQUIRE(polygon2Points.at(3).x() == multiPolygonFixture.x8 + translationPoint.x());
    REQUIRE(polygon2Points.at(3).y() == multiPolygonFixture.y8 + translationPoint.y());
    REQUIRE(polygon2Points.at(4).x() == multiPolygonFixture.x5 + translationPoint.x());
    REQUIRE(polygon2Points.at(4).y() == multiPolygonFixture.y5 + translationPoint.y());
}
