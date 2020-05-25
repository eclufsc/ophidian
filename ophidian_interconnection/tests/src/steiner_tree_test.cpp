#include <catch2/catch.hpp>
#include <ophidian/interconnection/SteinerTree.h>
#include <ophidian/geometry/Distance.h>

#include <memory>
#include <algorithm>

using namespace ophidian::interconnection;
using namespace ophidian::geometry;

TEST_CASE("Steiner Tree/empty steiner tree", "[interconnection]")
{
    auto tree = SteinerTree::create();
    CHECK( tree->size(SteinerTree::Segment{}) == 0 );
    CHECK( tree->size(SteinerTree::Point{}) == 0 );
}

TEST_CASE("Steiner Tree/add point", "[interconnection]")
{
    const auto kPosition = SteinerTree::DbuPoint{SteinerTree::dbu_t{1.0}, SteinerTree::dbu_t{2.0}};
    auto tree = SteinerTree::create();
    auto point = tree->add(kPosition);
    CHECK( tree->size(SteinerTree::Point{}) == 1 );
    CHECK( Approx(tree->position(point).x()) == kPosition.x() );
    CHECK( Approx(tree->position(point).y()) == kPosition.y() );
}

TEST_CASE("Steiner Tree/add segment", "[interconnection]")
{
    const auto kPositionP1 = SteinerTree::DbuPoint{SteinerTree::dbu_t{1.0}, SteinerTree::dbu_t{2.0}};
    const auto kPositionP2 = SteinerTree::DbuPoint{SteinerTree::dbu_t{100.0}, SteinerTree::dbu_t{200.0}};
    auto tree = SteinerTree::create();
    auto p1 = tree->add(kPositionP1);
    auto p2 = tree->add(kPositionP2);
    auto seg = tree->add(p1, p2);
    CHECK( tree->size(SteinerTree::Point{}) == 2 );
    CHECK( tree->size(SteinerTree::Segment{}) == 1 );
    auto checkUV = [](const SteinerTree & tree, const SteinerTree::Segment & seg, const SteinerTree::Point & point) -> bool{
        return tree.u(seg) == point ^ tree.v(seg) == point;
    };
    CHECK( checkUV(*tree, seg, p1) );
    CHECK( checkUV(*tree, seg, p2) );

    CHECK( Approx(tree->length(seg)) == ManhattanDistance(kPositionP1, kPositionP2) );
}

TEST_CASE("Steiner Tree/add same point twice", "[interconnection]")
{
    const auto kPosition = SteinerTree::DbuPoint{SteinerTree::dbu_t{1.0}, SteinerTree::dbu_t{2.0}};
    auto tree = SteinerTree::create();
    auto p1 = tree->add(kPosition);
    auto p2 = tree->add(kPosition);
    CHECK( p1 == p2 );
    CHECK( tree->size(SteinerTree::Point{}) == 1 );
}

TEST_CASE("Steiner Tree/geometry helper", "[interconnection]")
{
    const auto kPositionP1 = SteinerTree::DbuPoint{SteinerTree::dbu_t{1.0}, SteinerTree::dbu_t{2.0}};
    const auto kPositionP2 = SteinerTree::DbuPoint{SteinerTree::dbu_t{100.0}, SteinerTree::dbu_t{200.0}};
    const auto kSegment = make<ophidian::geometry::Segment, ophidian::geometry::Point, SteinerTree::dbu_t>({kPositionP1, kPositionP2});
    auto tree = SteinerTree::create();
    auto segment = tree->add(tree->add(kPositionP1), tree->add(kPositionP2));
    const auto geometrySegment = make_segment(*tree, segment);
    CHECK( Approx(geometrySegment.first.x()) == kSegment.first.x() );
    CHECK( Approx(geometrySegment.first.y()) == kSegment.first.y() );
    CHECK( Approx(geometrySegment.second.x()) == kSegment.second.x() );
    CHECK( Approx(geometrySegment.second.y()) == kSegment.second.y() );
}

TEST_CASE("Steiner Tree/steiner point iteration", "[interconnection]")
{
    const auto kPositionP1 = SteinerTree::DbuPoint{SteinerTree::dbu_t{1.0}, SteinerTree::dbu_t{2.0}};
    const auto kPositionP2 = SteinerTree::DbuPoint{SteinerTree::dbu_t{100.0}, SteinerTree::dbu_t{200.0}};
    auto tree = SteinerTree::create();
    auto p1 = SteinerTree::Point{};
    auto p2 = SteinerTree::Point{};
    auto segment = tree->add(p1 = tree->add(kPositionP1), p2 = tree->add(kPositionP2));
    CHECK( std::count(tree->points().first, tree->points().second, p1 ) == 1 );
    CHECK( std::count(tree->points().first, tree->points().second, p2 ) == 1 );
}

TEST_CASE("Steiner Tree/steiner point segments iteration", "[interconnection]")
{
    const auto kPositionP1 = SteinerTree::DbuPoint{SteinerTree::dbu_t{1.0}, SteinerTree::dbu_t{2.0}};
    const auto kPositionP2 = SteinerTree::DbuPoint{SteinerTree::dbu_t{100.0}, SteinerTree::dbu_t{200.0}};
    auto tree = SteinerTree::create();
    auto p1 = SteinerTree::Point{};
    auto p2 = SteinerTree::Point{};
    auto segment = tree->add(p1 = tree->add(kPositionP1), p2 = tree->add(kPositionP2));
    CHECK( std::count(tree->segments(p1).first, tree->segments(p1).second, segment) == 1 );
}

namespace {
class TreeWith4PointsFixture  {
public:
    static const SteinerTree::DbuPoint kPositionP1;
    static const SteinerTree::DbuPoint kPositionP2;
    static const SteinerTree::DbuPoint kPositionP3;
    static const SteinerTree::DbuPoint kPositionP4;

    TreeWith4PointsFixture() :
        tree_(SteinerTree::create())
    {
        points_[0] = tree_->add(kPositionP1);
        points_[1] = tree_->add(kPositionP2);
        points_[2] = tree_->add(kPositionP3);
        points_[3] = tree_->add(kPositionP4);
        segments_[0] = tree_->add(points_[0], points_[1]);
        segments_[1] = tree_->add(points_[0], points_[2]);
        segments_[2] = tree_->add(points_[2], points_[3]);
    }
    virtual ~TreeWith4PointsFixture()
    {

    }
protected:
    std::unique_ptr<SteinerTree> tree_;
    std::array<SteinerTree::Point, 4> points_;
    std::array<SteinerTree::Segment, 3> segments_;
};
}

const SteinerTree::DbuPoint TreeWith4PointsFixture::kPositionP1{SteinerTree::dbu_t{1.0}, SteinerTree::dbu_t{2.0}};
const SteinerTree::DbuPoint TreeWith4PointsFixture::kPositionP2{SteinerTree::dbu_t{100.0}, SteinerTree::dbu_t{200.0}};
const SteinerTree::DbuPoint TreeWith4PointsFixture::kPositionP3{SteinerTree::dbu_t{500.0}, SteinerTree::dbu_t{200.0}};
const SteinerTree::DbuPoint TreeWith4PointsFixture::kPositionP4{SteinerTree::dbu_t{200.0}, SteinerTree::dbu_t{200.0}};

TEST_CASE_METHOD(TreeWith4PointsFixture, "Steiner Tree/steiner point segments iteration2", "[interconnection]")
{
    auto p1Segs = tree_->segments(points_[0]);
    auto p2Segs = tree_->segments(points_[1]);
    auto p3Segs = tree_->segments(points_[2]);
    auto p4Segs = tree_->segments(points_[3]);
    CHECK( std::count(p1Segs.first, p1Segs.second, segments_[0]) == 1 );
    CHECK( std::count(p1Segs.first, p1Segs.second, segments_[1]) == 1 );
    CHECK( std::count(p2Segs.first, p2Segs.second, segments_[0]) == 1 );
    CHECK( std::count(p3Segs.first, p3Segs.second, segments_[1]) == 1 );
    CHECK( std::count(p3Segs.first, p3Segs.second, segments_[2]) == 1 );
    CHECK( std::count(p4Segs.first, p4Segs.second, segments_[2]) == 1 );
}

TEST_CASE_METHOD(TreeWith4PointsFixture, "Steiner Tree/steiner segments iteration", "[interconnection]")
{
    auto segments = tree_->segments();
    CHECK( std::distance(segments.first, segments.second) == tree_->size(SteinerTree::Segment{}) );
    CHECK( std::count(segments.first, segments.second, segments_[0]) == 1 );
    CHECK( std::count(segments.first, segments.second, segments_[1]) == 1 );
    CHECK( std::count(segments.first, segments.second, segments_[2]) == 1 );
}

TEST_CASE_METHOD(TreeWith4PointsFixture, "Steiner Tree/iterator general", "[interconnection]")
{
    SteinerTree::PointIterator it = tree_->points().first;
    const SteinerTree::PointIterator theEnd = tree_->points().second;
    for(;it != theEnd; ++it)
    {
    }
    auto it2 = std::next(it);
    CHECK(true);
}
