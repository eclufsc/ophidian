#include "steiner_tree_test.h"
#include <catch.hpp>
#include <ophidian/interconnection/SteinerTree.h>
#include <ophidian/geometry/Distance.h>

#include <memory>

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


TEST_CASE("Steiner Tree/geometry helper", "[interconnection]")
{
    const Point kPositionP1(1.0, 2.0);
    const Point kPositionP2(100.0, 200.0);
    const auto kSegment = make<Segment>({kPositionP1, kPositionP2});
    auto tree = std::make_unique<SteinerTree>();
    auto segment = tree->addSegment(tree->addPoint(kPositionP1), tree->addPoint(kPositionP2));

    const auto geometrySegment = make_segment(*tree, segment);

    REQUIRE( Approx(geometrySegment.first.x()) == kSegment.first.x() );
    REQUIRE( Approx(geometrySegment.first.y()) == kSegment.first.y() );
    REQUIRE( Approx(geometrySegment.second.x()) == kSegment.second.x() );
    REQUIRE( Approx(geometrySegment.second.y()) == kSegment.second.y() );
}

