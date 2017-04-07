#include <catch.hpp>

#include <ophidian/geometry/Operations.h>

#include "modelsfixture.h"

using namespace ophidian::geometry;

TEST_CASE("Geometry: scaling a Point", "[geometry][operations]") {
    Point scalingPoint(2.0, 2.0);

    PointFixture pointFixture;

    Point scaledPoint;
    scale(pointFixture.point, scalingPoint.x(), scalingPoint.y(), scaledPoint);

    REQUIRE(scaledPoint.x() == pointFixture.x * scalingPoint.x());
    REQUIRE(scaledPoint.y() == pointFixture.y * scalingPoint.y());
}

TEST_CASE("Geometry: scaling a Segment", "[geometry][operations]") {
    Point scalingPoint(2.0, 2.0);

    SegmentFixture segmentFixture;

    Segment scaledSegment;
    scale(segmentFixture.segment, scalingPoint.x(), scalingPoint.y(), scaledSegment);

    REQUIRE(scaledSegment.first.x() == segmentFixture.x1 * scalingPoint.x());
    REQUIRE(scaledSegment.first.y() == segmentFixture.y1 * scalingPoint.y());
    REQUIRE(scaledSegment.second.x() == segmentFixture.x2 * scalingPoint.x());
    REQUIRE(scaledSegment.second.y() == segmentFixture.y2 * scalingPoint.y());
}

TEST_CASE("Geometry: scaling a Linestring", "[geometry][operations]") {
    Point scalingPoint(2.0, 2.0);

    LinestringFixture linestringFixture;

    Linestring scaledLinestring;
    scale(linestringFixture.linestring, scalingPoint.x(), scalingPoint.y(), scaledLinestring);

    std::vector<Point> linestringPoints(scaledLinestring.begin(), scaledLinestring.end());
    REQUIRE(linestringPoints.size() == 3);
    REQUIRE(linestringPoints.at(0).x() == linestringFixture.x1 * scalingPoint.x());
    REQUIRE(linestringPoints.at(0).y() == linestringFixture.y1 * scalingPoint.y());
    REQUIRE(linestringPoints.at(1).x() == linestringFixture.x2 * scalingPoint.x());
    REQUIRE(linestringPoints.at(1).y() == linestringFixture.y2 * scalingPoint.y());
    REQUIRE(linestringPoints.at(2).x() == linestringFixture.x3 * scalingPoint.x());
    REQUIRE(linestringPoints.at(2).y() == linestringFixture.y3 * scalingPoint.y());
}

TEST_CASE("Geometry: scaling a Box", "[geometry][operations]") {
    Point scalingPoint(2.0, 2.0);

    BoxFixture boxFixture;

    Box scaledBox;
    scale(boxFixture.box, scalingPoint.x(), scalingPoint.y(), scaledBox);

    REQUIRE(scaledBox.min_corner().x() == boxFixture.xMin * scalingPoint.x());
    REQUIRE(scaledBox.min_corner().y() == boxFixture.yMin * scalingPoint.y());
    REQUIRE(scaledBox.max_corner().x() == boxFixture.xMax * scalingPoint.x());
    REQUIRE(scaledBox.max_corner().y() == boxFixture.yMax * scalingPoint.y());
}

TEST_CASE("Geometry: scaling a Polygon", "[geometry][operations]") {
    Point scalingPoint(2.0, 2.0);

    PolygonFixture polygonFixture;

    Polygon scaledPolygon;
    scale(polygonFixture.polygon, scalingPoint.x(), scalingPoint.y(), scaledPolygon);

    std::vector<Point> polygonPoints(scaledPolygon.outer().begin(), scaledPolygon.outer().end());
    REQUIRE(polygonPoints.size() == 5);
    REQUIRE(polygonPoints.at(0).x() == polygonFixture.x1 * scalingPoint.x());
    REQUIRE(polygonPoints.at(0).y() == polygonFixture.y1 * scalingPoint.y());
    REQUIRE(polygonPoints.at(1).x() == polygonFixture.x2 * scalingPoint.x());
    REQUIRE(polygonPoints.at(1).y() == polygonFixture.y2 * scalingPoint.y());
    REQUIRE(polygonPoints.at(2).x() == polygonFixture.x3 * scalingPoint.x());
    REQUIRE(polygonPoints.at(2).y() == polygonFixture.y3 * scalingPoint.y());
    REQUIRE(polygonPoints.at(3).x() == polygonFixture.x4 * scalingPoint.x());
    REQUIRE(polygonPoints.at(3).y() == polygonFixture.y4 * scalingPoint.y());
    REQUIRE(polygonPoints.at(4).x() == polygonFixture.x1 * scalingPoint.x());
    REQUIRE(polygonPoints.at(4).y() == polygonFixture.y1 * scalingPoint.y());
}

TEST_CASE("Geometry: scaling a MultiPolygon", "[geometry][operations]") {
    Point scalingPoint(2.0, 2.0);

    MultiPolygonFixture multiPolygonFixture;

    MultiPolygon scaledMultiPolygon;
    scale(multiPolygonFixture.multiPolygon, scalingPoint.x(), scalingPoint.y(), scaledMultiPolygon);

    std::vector<Point> polygon1Points(scaledMultiPolygon.at(0).outer().begin(), scaledMultiPolygon.at(0).outer().end());
    REQUIRE(polygon1Points.size() == 5);
    REQUIRE(polygon1Points.at(0).x() == multiPolygonFixture.x1 * scalingPoint.x());
    REQUIRE(polygon1Points.at(0).y() == multiPolygonFixture.y1 * scalingPoint.y());
    REQUIRE(polygon1Points.at(1).x() == multiPolygonFixture.x2 * scalingPoint.x());
    REQUIRE(polygon1Points.at(1).y() == multiPolygonFixture.y2 * scalingPoint.y());
    REQUIRE(polygon1Points.at(2).x() == multiPolygonFixture.x3 * scalingPoint.x());
    REQUIRE(polygon1Points.at(2).y() == multiPolygonFixture.y3 * scalingPoint.y());
    REQUIRE(polygon1Points.at(3).x() == multiPolygonFixture.x4 * scalingPoint.x());
    REQUIRE(polygon1Points.at(3).y() == multiPolygonFixture.y4 * scalingPoint.y());
    REQUIRE(polygon1Points.at(4).x() == multiPolygonFixture.x1 * scalingPoint.x());
    REQUIRE(polygon1Points.at(4).y() == multiPolygonFixture.y1 * scalingPoint.y());

    std::vector<Point> polygon2Points(scaledMultiPolygon.at(1).outer().begin(), scaledMultiPolygon.at(1).outer().end());
    REQUIRE(polygon2Points.size() == 5);
    REQUIRE(polygon2Points.at(0).x() == multiPolygonFixture.x5 * scalingPoint.x());
    REQUIRE(polygon2Points.at(0).y() == multiPolygonFixture.y5 * scalingPoint.y());
    REQUIRE(polygon2Points.at(1).x() == multiPolygonFixture.x6 * scalingPoint.x());
    REQUIRE(polygon2Points.at(1).y() == multiPolygonFixture.y6 * scalingPoint.y());
    REQUIRE(polygon2Points.at(2).x() == multiPolygonFixture.x7 * scalingPoint.x());
    REQUIRE(polygon2Points.at(2).y() == multiPolygonFixture.y7 * scalingPoint.y());
    REQUIRE(polygon2Points.at(3).x() == multiPolygonFixture.x8 * scalingPoint.x());
    REQUIRE(polygon2Points.at(3).y() == multiPolygonFixture.y8 * scalingPoint.y());
    REQUIRE(polygon2Points.at(4).x() == multiPolygonFixture.x5 * scalingPoint.x());
    REQUIRE(polygon2Points.at(4).y() == multiPolygonFixture.y5 * scalingPoint.y());
}
