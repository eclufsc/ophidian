#include "modelsfixture.h"

// using namespace ophidian::geometry;
//
// PointFixture::PointFixture()
//     : point(x, y){
//
// }
//
// SegmentFixture::SegmentFixture() {
//     Point segmentBegin(x1, y1);
//     Point segmentEnd(x2, y2);
//     segment = Segment(segmentBegin, segmentEnd);
// }
//
// LinestringFixture::LinestringFixture()
// {
//     std::vector<Point> points = {
//         Point(x1, y1),
//         Point(x2, y2),
//         Point(x3, y3),
//     };
//     linestring = make<Linestring>(points);
// }
//
// BoxFixture::BoxFixture()
// {
//     box = Box(Point(xMin, yMin), Point(xMax, yMax));
// }
//
// PolygonFixture::PolygonFixture()
// {
//     std::vector<Point> points = {
//         Point(x1, y1),
//         Point(x2, y2),
//         Point(x3, y3),
//         Point(x4, y4),
//         Point(x1, y1),
//     };
//     polygon = make<Polygon>(points);
// }
//
// MultiPolygonFixture::MultiPolygonFixture()
// {
//     std::vector<Point> points1 = {
//         Point(x1, y1),
//         Point(x2, y2),
//         Point(x3, y3),
//         Point(x4, y4),
//         Point(x1, y1),
//     };
//     Polygon polygon1 = make<Polygon>(points1);
//
//     std::vector<Point> points2 = {
//         Point(x5, y5),
//         Point(x6, y6),
//         Point(x7, y7),
//         Point(x8, y8),
//         Point(x5, y5),
//     };
//     Polygon polygon2 = make<Polygon>(points2);
//
//     std::vector<Polygon> polygons = {
//         polygon1,
//         polygon2
//     };
//     multiPolygon = makeMulti<MultiPolygon>(polygons);
// }
