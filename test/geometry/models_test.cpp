#include <catch.hpp>

#include <ophidian/geometry/Models.h>

using namespace ophidian::geometry;

TEST_CASE("Geometry: new Point must have correct coordinate values", "[geometry][models]") {
    double x = 1.0;
    double y = 2.0;
    Point point(x, y);
    REQUIRE(point.x() == x);
    REQUIRE(point.y() == y);
}

TEST_CASE("Geometry: new Segment must have correct coordinate values", "[geometry][models]") {
    double x1 = 1.0;
    double y1 = 2.0;
    double x2 = 2.0;
    double y2 = 1.0;
    Point segmentBegin(x1, y1);
    Point segmentEnd(x2, y2);
    Segment segment(segmentBegin, segmentEnd);
    REQUIRE(segment.first.x() == x1);
    REQUIRE(segment.first.y() == y1);
    REQUIRE(segment.second.x() == x2);
    REQUIRE(segment.second.y() == y2);
}

TEST_CASE("Geometry: new Linestring must have correct coordinate values", "[geometry][models]") {
    double x1 = 1.0;
    double y1 = 2.0;
    double x2 = 2.0;
    double y2 = 1.0;
    double x3 = 2.0;
    double y3 = 1.0;
    std::vector<Point> points = {
        Point(x1, y1),
        Point(x2, y2),
        Point(x3, y3),
    };
    Linestring linestring(points.begin(), points.end());
    std::vector<Point> linestringPoints(linestring.begin(), linestring.end());
    REQUIRE(linestringPoints.size() == 3);
    REQUIRE(linestringPoints.at(0).x() == x1);
    REQUIRE(linestringPoints.at(0).y() == y1);
    REQUIRE(linestringPoints.at(1).x() == x2);
    REQUIRE(linestringPoints.at(1).y() == y2);
    REQUIRE(linestringPoints.at(2).x() == x3);
    REQUIRE(linestringPoints.at(2).y() == y3);
}

TEST_CASE("Geometry: new Box must have correct coordinate values", "[geometry][models]") {
    double xMin = 1.0;
    double yMin = 2.0;
    double xMax = 2.0;
    double yMax = 2.0;
    Point minCorner(xMin, yMin);
    Point maxCorner(xMax, yMax);
    Box box(minCorner, maxCorner);
    REQUIRE(box.min_corner().x() == xMin);
    REQUIRE(box.min_corner().y() == yMin);
    REQUIRE(box.max_corner().x() == xMax);
    REQUIRE(box.max_corner().y() == yMax);
}

TEST_CASE("Geometry: new Polygon must have correct coordinate values", "[geometry][models]") {
    double x1 = 1.0;
    double y1 = 1.0;
    double x2 = 2.0;
    double y2 = 1.0;
    double x3 = 2.0;
    double y3 = 2.0;
    double x4 = 1.0;
    double y4 = 2.0;
    std::vector<Point> points = {
        Point(x1, y1),
        Point(x2, y2),
        Point(x3, y3),
        Point(x4, y4),
        Point(x1, y1),
    };
    Polygon polygon = make<Polygon>(points);
    std::vector<Point> polygonPoints(polygon.outer().begin(), polygon.outer().end());
    REQUIRE(polygonPoints.size() == 5);
    REQUIRE(polygonPoints.at(0).x() == x1);
    REQUIRE(polygonPoints.at(0).y() == y1);
    REQUIRE(polygonPoints.at(1).x() == x2);
    REQUIRE(polygonPoints.at(1).y() == y2);
    REQUIRE(polygonPoints.at(2).x() == x3);
    REQUIRE(polygonPoints.at(2).y() == y3);
    REQUIRE(polygonPoints.at(3).x() == x4);
    REQUIRE(polygonPoints.at(3).y() == y4);
    REQUIRE(polygonPoints.at(4).x() == x1);
    REQUIRE(polygonPoints.at(4).y() == y1);
}

TEST_CASE("Geometry: new MultiPolygon must have correct coordinate values", "[geometry][models]") {
    double x1 = 1.0;
    double y1 = 1.0;
    double x2 = 2.0;
    double y2 = 1.0;
    double x3 = 2.0;
    double y3 = 2.0;
    double x4 = 1.0;
    double y4 = 2.0;
    std::vector<Point> points1 = {
        Point(x1, y1),
        Point(x2, y2),
        Point(x3, y3),
        Point(x4, y4),
        Point(x1, y1),
    };
    Polygon polygon1 = make<Polygon>(points1);

    double x5 = 1.0;
    double y5 = 1.0;
    double x6 = 2.0;
    double y6 = 1.0;
    double x7 = 2.0;
    double y7 = 2.0;
    double x8 = 1.0;
    double y8 = 2.0;
    std::vector<Point> points2 = {
        Point(x5, y5),
        Point(x6, y6),
        Point(x7, y7),
        Point(x8, y8),
        Point(x5, y5),
    };
    Polygon polygon2 = make<Polygon>(points2);

    std::vector<Polygon> polygons = {
        polygon1,
        polygon2
    };
    MultiPolygon multiPolygon = makeMulti<MultiPolygon>(polygons);

    std::vector<Point> polygon1Points(multiPolygon.at(0).outer().begin(), multiPolygon.at(0).outer().end());
    REQUIRE(polygon1Points.size() == 5);
    REQUIRE(polygon1Points.at(0).x() == x1);
    REQUIRE(polygon1Points.at(0).y() == y1);
    REQUIRE(polygon1Points.at(1).x() == x2);
    REQUIRE(polygon1Points.at(1).y() == y2);
    REQUIRE(polygon1Points.at(2).x() == x3);
    REQUIRE(polygon1Points.at(2).y() == y3);
    REQUIRE(polygon1Points.at(3).x() == x4);
    REQUIRE(polygon1Points.at(3).y() == y4);
    REQUIRE(polygon1Points.at(4).x() == x1);
    REQUIRE(polygon1Points.at(4).y() == y1);

    std::vector<Point> polygon2Points(multiPolygon.at(1).outer().begin(), multiPolygon.at(1).outer().end());
    REQUIRE(polygon2Points.size() == 5);
    REQUIRE(polygon2Points.at(0).x() == x5);
    REQUIRE(polygon2Points.at(0).y() == y5);
    REQUIRE(polygon2Points.at(1).x() == x6);
    REQUIRE(polygon2Points.at(1).y() == y6);
    REQUIRE(polygon2Points.at(2).x() == x7);
    REQUIRE(polygon2Points.at(2).y() == y7);
    REQUIRE(polygon2Points.at(3).x() == x8);
    REQUIRE(polygon2Points.at(3).y() == y8);
    REQUIRE(polygon2Points.at(4).x() == x5);
    REQUIRE(polygon2Points.at(4).y() == y5);
}
