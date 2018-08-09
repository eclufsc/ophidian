#include <catch.hpp>
#include <ophidian/interconnection/SteinerTree.h>
#include <ophidian/geometry/Distance.h>

#include <memory>
#include <algorithm>

using namespace ophidian::interconnection;
using namespace ophidian::geometry;

// TEST_CASE("Steiner Tree/empty steiner tree", "[interconnection]")
// {
//     auto tree = SteinerTree::create();
//     REQUIRE( tree->size(SteinerTree::Segment{}) == 0 );
//     REQUIRE( tree->size(SteinerTree::Point{}) == 0 );
// }
//
// TEST_CASE("Steiner Tree/add point", "[interconnection]")
// {
//     const Point kPosition(1.0, 2.0);
//     auto tree = SteinerTree::create();
//     auto point = tree->add(kPosition);
//     REQUIRE( tree->size(SteinerTree::Point{}) == 1 );
//     REQUIRE( Approx(tree->position(point).x()) == kPosition.x() );
//     REQUIRE( Approx(tree->position(point).y()) == kPosition.y() );
// }
//
// TEST_CASE("Steiner Tree/add segment", "[interconnection]")
// {
//     const Point kPositionP1(1.0, 2.0);
//     const Point kPositionP2(100.0, 200.0);
//     auto tree = SteinerTree::create();
//     auto p1 = tree->add(kPositionP1);
//     auto p2 = tree->add(kPositionP2);
//     auto seg = tree->add(p1, p2);
//     REQUIRE( tree->size(SteinerTree::Point{}) == 2 );
//     REQUIRE( tree->size(SteinerTree::Segment{}) == 1 );
//     auto checkUV = [](const SteinerTree & tree, const SteinerTree::Segment & seg, const SteinerTree::Point & point) -> bool{
//         return tree.u(seg) == point ^ tree.v(seg) == point;
//     };
//     REQUIRE( checkUV(*tree, seg, p1) );
//     REQUIRE( checkUV(*tree, seg, p2) );
//     ManhattanDistance distance;
//     REQUIRE( Approx(tree->length(seg)) == distance(kPositionP1, kPositionP2) );
// }
//
// TEST_CASE("Steiner Tree/add same point twice", "[interconnection]")
// {
//     const Point kPosition(1.0, 2.0);
//     auto tree = SteinerTree::create();
//     auto p1 = tree->add(kPosition);
//     auto p2 = tree->add(kPosition);
//     REQUIRE( p1 == p2 );
//     REQUIRE( tree->size(SteinerTree::Point{}) == 1 );
// }
//
// TEST_CASE("Steiner Tree/geometry helper", "[interconnection]")
// {
//     const Point kPositionP1(1.0, 2.0);
//     const Point kPositionP2(100.0, 200.0);
//     const auto kSegment = make<Segment>({kPositionP1, kPositionP2});
//     auto tree = SteinerTree::create();
//     auto segment = tree->add(tree->add(kPositionP1), tree->add(kPositionP2));
//     const auto geometrySegment = make<Segment>(*tree, segment);
//     REQUIRE( Approx(geometrySegment.first.x()) == kSegment.first.x() );
//     REQUIRE( Approx(geometrySegment.first.y()) == kSegment.first.y() );
//     REQUIRE( Approx(geometrySegment.second.x()) == kSegment.second.x() );
//     REQUIRE( Approx(geometrySegment.second.y()) == kSegment.second.y() );
// }
//
//
// TEST_CASE("Steiner Tree/steiner point iteration", "[interconnection]")
// {
//     const Point kPositionP1(1.0, 2.0);
//     const Point kPositionP2(100.0, 200.0);
//     auto tree = SteinerTree::create();
//     SteinerTree::Point p1, p2;
//     auto segment = tree->add(p1 = tree->add(kPositionP1), p2 = tree->add(kPositionP2));
//     REQUIRE( std::count(tree->points().first, tree->points().second, p1 ) == 1 );
//     REQUIRE( std::count(tree->points().first, tree->points().second, p2 ) == 1 );
// }
//
// TEST_CASE("Steiner Tree/steiner point segments iteration", "[interconnection]")
// {
//     const Point kPositionP1(1.0, 2.0);
//     const Point kPositionP2(100.0, 200.0);
//     auto tree = SteinerTree::create();
//     SteinerTree::Point p1, p2;
//     auto segment = tree->add(p1 = tree->add(kPositionP1), p2 = tree->add(kPositionP2));
//     REQUIRE( std::count(tree->segments(p1).first, tree->segments(p1).second, segment) == 1 );
// }
//
// namespace {
// class TreeWith4PointsFixture  {
// public:
//     static const Point kPositionP1;
//     static const Point kPositionP2;
//     static const Point kPositionP3;
//     static const Point kPositionP4;
//
//     TreeWith4PointsFixture() :
//         tree_(SteinerTree::create())
//     {
//         points_[0] = tree_->add(kPositionP1);
//         points_[1] = tree_->add(kPositionP2);
//         points_[2] = tree_->add(kPositionP3);
//         points_[3] = tree_->add(kPositionP4);
//         segments_[0] = tree_->add(points_[0], points_[1]);
//         segments_[1] = tree_->add(points_[0], points_[2]);
//         segments_[2] = tree_->add(points_[2], points_[3]);
//     }
//     virtual ~TreeWith4PointsFixture()
//     {
//
//     }
// protected:
//     std::unique_ptr<SteinerTree> tree_;
//     std::array<SteinerTree::Point, 4> points_;
//     std::array<SteinerTree::Segment, 3> segments_;
// };
// }
//
// const Point TreeWith4PointsFixture::kPositionP1{1.0, 2.0};
// const Point TreeWith4PointsFixture::kPositionP2{100.0, 200.0};
// const Point TreeWith4PointsFixture::kPositionP3{500.0, 200.0};
// const Point TreeWith4PointsFixture::kPositionP4{200.0, 200.0};
//
//
// TEST_CASE_METHOD(TreeWith4PointsFixture, "Steiner Tree/steiner point segments iteration2", "[interconnection]")
// {
//     auto p1Segs = tree_->segments(points_[0]);
//     auto p2Segs = tree_->segments(points_[1]);
//     auto p3Segs = tree_->segments(points_[2]);
//     auto p4Segs = tree_->segments(points_[3]);
//     REQUIRE( std::count(p1Segs.first, p1Segs.second, segments_[0]) == 1 );
//     REQUIRE( std::count(p1Segs.first, p1Segs.second, segments_[1]) == 1 );
//     REQUIRE( std::count(p2Segs.first, p2Segs.second, segments_[0]) == 1 );
//     REQUIRE( std::count(p3Segs.first, p3Segs.second, segments_[1]) == 1 );
//     REQUIRE( std::count(p3Segs.first, p3Segs.second, segments_[2]) == 1 );
//     REQUIRE( std::count(p4Segs.first, p4Segs.second, segments_[2]) == 1 );
// }
//
// TEST_CASE_METHOD(TreeWith4PointsFixture, "Steiner Tree/steiner segments iteration", "[interconnection]")
// {
//     auto segments = tree_->segments();
//     REQUIRE( std::distance(segments.first, segments.second) == tree_->size(SteinerTree::Segment{}) );
//     REQUIRE( std::count(segments.first, segments.second, segments_[0]) == 1 );
//     REQUIRE( std::count(segments.first, segments.second, segments_[1]) == 1 );
//     REQUIRE( std::count(segments.first, segments.second, segments_[2]) == 1 );
// }
//
// TEST_CASE_METHOD(TreeWith4PointsFixture, "Steiner Tree/iterator general", "[interconnection]")
// {
//     SteinerTree::PointIterator it = tree_->points().first;
//     const SteinerTree::PointIterator theEnd = tree_->points().second;
//     for(;it != theEnd; ++it)
//     {
//     }
//     auto it2 = std::next(it);
//     REQUIRE(true);
// }
