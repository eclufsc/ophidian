#include "SteinerTree.h"
#include <ophidian/geometry/Distance.h>

namespace ophidian {
namespace interconnection {

SteinerTree::SteinerTree() :
    position_(graph_)
{

}

uint32_t SteinerTree::numSegments() const
{
    return lemon::countEdges(graph_);
}

uint32_t SteinerTree::numPoints() const
{
    return lemon::countNodes(graph_);
}


namespace {
lemon::SmartGraph::Node findNodeWithPositionEqualsTo(const geometry::Point &position, const lemon::SmartGraph::NodeMap<geometry::Point> & position_, const lemon::SmartGraph & graph_)
{
    geometry::ManhattanDistance distance;
    for(lemon::SmartGraph::NodeIt i(graph_); graph_.valid(i); ++i)
    {
        if(distance(position, position_[i]) == 0.0)
        {
            return i;
        }
    }
    return lemon::INVALID;
}
}

SteinerTree::Point SteinerTree::addPoint(const geometry::Point &position)
{
    lemon::SmartGraph::Node node = findNodeWithPositionEqualsTo(position, position_, graph_);
    if(node == lemon::INVALID)
    {
        node = graph_.addNode();
        position_[node] = position;
    }
    return Point(node);
}

SteinerTree::Segment SteinerTree::addSegment(const SteinerTree::Point &p1, const SteinerTree::Point &p2)
{
    auto edge = graph_.addEdge(p1.node_, p2.node_);
    return Segment(edge);
}

SteinerTree::Point SteinerTree::u(const SteinerTree::Segment &segment) const
{
    return Point(graph_.u(segment.edge_));
}

SteinerTree::Point SteinerTree::v(const SteinerTree::Segment &segment) const
{
    return Point(graph_.v(segment.edge_));
}

geometry::Point SteinerTree::position(const SteinerTree::Point &p) const
{
    return position_[p.node_];
}

double SteinerTree::length(const SteinerTree::Segment &segment) const
{
    const auto kU = graph_.u(segment.edge_);
    const auto kV = graph_.v(segment.edge_);
    geometry::ManhattanDistance distance;
    return distance(position_[kU], position_[kV]);
}

SteinerTree::Point::Point(lemon::SmartGraph::Node node) :
    node_(node)
{

}

SteinerTree::Segment::Segment(lemon::SmartGraph::Edge edge) :
    edge_(edge)
{

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

