#include "SteinerTree.h"
#include <ophidian/geometry/Distance.h>
#include <numeric>
#include "ToEps.h"
#include <lemon/graph_to_eps.h>

namespace ophidian {
namespace interconnection {

namespace {
inline geometry::Point convert(const lemon::dim2::Point<double> & p) {
    return {p.x, p.y};
}
inline lemon::dim2::Point<double> convert(const geometry::Point & p) {
    return {p.x(), p.y()};
}
}

SteinerTree::SteinerTree() :
    position_(graph_)
{

}

uint32_t SteinerTree::size(Segment) const
{
    return lemon::countEdges(graph_);
}

uint32_t SteinerTree::size(Point) const
{
    return lemon::countNodes(graph_);
}

namespace {
SteinerTree::GraphType::Node findNodeWithPositionEqualsTo(const geometry::Point &position, const SteinerTree::GraphType::NodeMap<lemon::dim2::Point<double> > & position_, const SteinerTree::GraphType & graph_)
{
    geometry::ManhattanDistance distance;
    for(SteinerTree::GraphType::NodeIt i(graph_); graph_.valid(i); ++i)
    {
        if(distance(position, convert(position_[i])) == 0.0)
        {
            return i;
        }
    }
    return lemon::INVALID;
}
}

std::unique_ptr<SteinerTree> SteinerTree::create()
{
    return std::unique_ptr<SteinerTree>{new SteinerTree};
}

SteinerTree::Point SteinerTree::add(const geometry::Point &position)
{
    GraphType::Node node = findNodeWithPositionEqualsTo(position, position_, graph_);
    if(node == lemon::INVALID)
    {
        node = graph_.addNode();
        position_[node] = convert(position);
    }
    return Point(node);
}

SteinerTree::Segment SteinerTree::add(const SteinerTree::Point &p1, const SteinerTree::Point &p2)
{
    auto edge = graph_.addEdge(p1.el_, p2.el_);
    Segment result(edge);
    length_ += length(result);
    return result;
}

SteinerTree::Point SteinerTree::u(const SteinerTree::Segment &segment) const
{
    return Point(graph_.u(segment.el_));
}

SteinerTree::Point SteinerTree::v(const SteinerTree::Segment &segment) const
{
    return Point(graph_.v(segment.el_));
}

geometry::Point SteinerTree::position(const SteinerTree::Point &p) const
{
    return convert(position_[p.el_]);
}

double SteinerTree::length(const SteinerTree::Segment &segment) const
{
    const auto kU = graph_.u(segment.el_);
    const auto kV = graph_.v(segment.el_);
    geometry::ManhattanDistance distance;
    return distance(convert(position_[kU]), convert(position_[kV]));
}

double SteinerTree::length() const
{
    return length_;
}

std::pair<SteinerTree::PointIterator, SteinerTree::PointIterator> SteinerTree::points() const
{
    PointIterator first {Point {GraphType::NodeIt {graph_}}};
    PointIterator second {Point {static_cast<GraphType::Node>(lemon::INVALID)}};
    return std::make_pair(first, second);
}

std::pair<SteinerTree::PointSegmentsIterator, SteinerTree::PointSegmentsIterator> SteinerTree::segments(const SteinerTree::Point &point) const
{
    PointSegmentsIterator first {{graph_, point.el_}};
    PointSegmentsIterator second {lemon::INVALID};
    return std::make_pair(first, second);
}

std::pair<SteinerTree::SegmentIterator, SteinerTree::SegmentIterator> SteinerTree::segments() const
{
    SegmentIterator first {GraphType::EdgeIt {graph_}};
    SegmentIterator second {lemon::INVALID};
    return std::make_pair(first, second);
}

SteinerTree::PointIterator::PointIterator(const SteinerTree::Point &p) :
    point_(p)
{

}

SteinerTree::PointSegmentsIterator::PointSegmentsIterator(GraphType::IncEdgeIt it) :
    it_(it)
{

}

SteinerTree::SegmentIterator::SegmentIterator(GraphType::EdgeIt it) :
    it_(it)
{

}

namespace {
class SteinerTreeToEps : public SteinerTree::Attorney
{
public:
    static void run(const SteinerTree & tree, const std::string &filename)
    {
        const SteinerTree::GraphType & graph = SteinerTree::Attorney::graph(tree);
        auto & coords = SteinerTree::Attorney::position(tree);
        SteinerTree::GraphType::NodeMap<int> color(graph);
        lemon::Palette palette;
        int i = 0;
        for(SteinerTree::GraphType::NodeIt curr(graph); curr != lemon::INVALID; ++curr)
        {
            color[curr] = 1 + (i % 4);
            ++i;
        }
        lemon::graphToEps(graph, filename).coords(coords).nodeColors(lemon::composeMap(palette, color)).run();
    }
};
}

template <>
void ToEps::run<SteinerTree>(const SteinerTree & tree, const std::string &filename)
{
    SteinerTreeToEps::run(tree, filename);
}

}

namespace geometry
{
template <>
geometry::Segment make<geometry::Segment>(const interconnection::SteinerTree & tree, const interconnection::SteinerTree::Segment & segment)
{
    return make<geometry::Segment>({tree.position(tree.u(segment)), tree.position(tree.v(segment))});
}
}

}

