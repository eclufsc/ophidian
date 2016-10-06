#include "steiner_tree_test.h"
#include <catch.hpp>
#include <ophidian/interconnection/SteinerTree.h>
#include <ophidian/geometry/Distance.h>

#include <memory>
#include <algorithm>

using namespace ophidian::interconnection;
using namespace ophidian::geometry;

TEST_CASE("Steiner Tree/empty steiner tree", "[interconnection]")
{
    auto tree = std::make_unique<SteinerTree>();
    REQUIRE( tree->numSegments() == 0 );
    REQUIRE( tree->numPoints() == 0 );
}


TEST_CASE("Steiner Tree/add point", "[interconnection]")
{
    const Point kPosition(1.0, 2.0);
    auto tree = std::make_unique<SteinerTree>();
    auto point = tree->addPoint(kPosition);
    REQUIRE( tree->numPoints() == 1 );
    REQUIRE( Approx(tree->position(point).x()) == kPosition.x() );
    REQUIRE( Approx(tree->position(point).y()) == kPosition.y() );
}

TEST_CASE("Steiner Tree/add segment", "[interconnection]")
{
    const Point kPositionP1(1.0, 2.0);
    const Point kPositionP2(100.0, 200.0);
    auto tree = std::make_unique<SteinerTree>();
    auto p1 = tree->addPoint(kPositionP1);
    auto p2 = tree->addPoint(kPositionP2);
    auto seg = tree->addSegment(p1, p2);
    REQUIRE( tree->numPoints() == 2 );
    REQUIRE( tree->numSegments() == 1 );
    auto checkUV = [](const SteinerTree & tree, const SteinerTree::Segment & seg, const SteinerTree::Point & point) -> bool{
        return tree.u(seg) == point ^ tree.v(seg) == point;
    };
    REQUIRE( checkUV(*tree, seg, p1) );
    REQUIRE( checkUV(*tree, seg, p2) );
    ManhattanDistance distance;
    REQUIRE( Approx(tree->length(seg)) == distance(kPositionP1, kPositionP2) );
}

TEST_CASE("Steiner Tree/add same point twice", "[interconnection]")
{
    const Point kPosition(1.0, 2.0);
    auto tree = std::make_unique<SteinerTree>();
    auto p1 = tree->addPoint(kPosition);
    auto p2 = tree->addPoint(kPosition);
    REQUIRE( p1 == p2 );
    REQUIRE( tree->numPoints() == 1 );
}

TEST_CASE("Steiner Tree/move point", "[interconnection]")
{
    const Point kPosition(1.0, 2.0);
    const Point kPositionF(3.0, 4.0);
    auto tree = std::make_unique<SteinerTree>();
    auto point = tree->addPoint(kPosition);
    tree->position(point, kPositionF);
    REQUIRE( Approx(tree->position(point).x()) == kPositionF.x() );
    REQUIRE( Approx(tree->position(point).y()) == kPositionF.y() );
}


TEST_CASE("Steiner Tree/geometry helper", "[interconnection]")
{
    const Point kPositionP1(1.0, 2.0);
    const Point kPositionP2(100.0, 200.0);
    const auto kSegment = make<Segment>({kPositionP1, kPositionP2});
    auto tree = std::make_unique<SteinerTree>();
    auto segment = tree->addSegment(tree->addPoint(kPositionP1), tree->addPoint(kPositionP2));

    const auto geometrySegment = make<Segment>(*tree, segment);

    REQUIRE( Approx(geometrySegment.first.x()) == kSegment.first.x() );
    REQUIRE( Approx(geometrySegment.first.y()) == kSegment.first.y() );
    REQUIRE( Approx(geometrySegment.second.x()) == kSegment.second.x() );
    REQUIRE( Approx(geometrySegment.second.y()) == kSegment.second.y() );
}


TEST_CASE("Steiner Tree/steiner point iteration", "[interconnection]")
{
    const Point kPositionP1(1.0, 2.0);
    const Point kPositionP2(100.0, 200.0);
    auto tree = std::make_unique<SteinerTree>();
    SteinerTree::Point p1, p2;
    auto segment = tree->addSegment(p1 = tree->addPoint(kPositionP1), p2 = tree->addPoint(kPositionP2));
    REQUIRE( std::count(tree->points().first, tree->points().second, p1 ) == 1 );
    REQUIRE( std::count(tree->points().first, tree->points().second, p2 ) == 1 );
}

TEST_CASE("Steiner Tree/steiner point segments iteration", "[interconnection]")
{
    const Point kPositionP1(1.0, 2.0);
    const Point kPositionP2(100.0, 200.0);
    auto tree = std::make_unique<SteinerTree>();
    SteinerTree::Point p1, p2;
    auto segment = tree->addSegment(p1 = tree->addPoint(kPositionP1), p2 = tree->addPoint(kPositionP2));
    REQUIRE( std::count(tree->segments(p1).first, tree->segments(p1).second, segment) == 1 );
}


TEST_CASE("Steiner Tree/steiner point segments iteration2", "[interconnection]")
{
    const Point kPositionP1(1.0, 2.0);
    const Point kPositionP2(100.0, 200.0);
    const Point kPositionP3(500.0, 200.0);
    const Point kPositionP4(200.0, 200.0);
    auto tree = std::make_unique<SteinerTree>();
    SteinerTree::Point p1{tree->addPoint(kPositionP1)};
    SteinerTree::Point p2{tree->addPoint(kPositionP2)};
    SteinerTree::Point p3{tree->addPoint(kPositionP3)};
    SteinerTree::Point p4{tree->addPoint(kPositionP4)};
    auto seg1 = tree->addSegment(p1, p2);
    auto seg2 = tree->addSegment(p1, p3);
    auto seg3 = tree->addSegment(p3, p4);
    auto p1Segs = tree->segments(p1);
    auto p2Segs = tree->segments(p2);
    auto p3Segs = tree->segments(p3);
    auto p4Segs = tree->segments(p4);
    REQUIRE( std::count(p1Segs.first, p1Segs.second, seg1) == 1 );
    REQUIRE( std::count(p1Segs.first, p1Segs.second, seg2) == 1 );
    REQUIRE( std::count(p2Segs.first, p2Segs.second, seg1) == 1 );
    REQUIRE( std::count(p3Segs.first, p3Segs.second, seg2) == 1 );
    REQUIRE( std::count(p3Segs.first, p3Segs.second, seg3) == 1 );
    REQUIRE( std::count(p4Segs.first, p4Segs.second, seg3) == 1 );
}
