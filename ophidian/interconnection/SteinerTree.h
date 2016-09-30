#ifndef STEINERTREE_H
#define STEINERTREE_H

#include <ophidian/geometry/Models.h>
#include <lemon/smart_graph.h>
#include <cstdint>
#include <memory>

namespace ophidian {
namespace interconnection {

class SteinerTree
{
public:
    class Point final {
    public:
        friend class SteinerTree;
        bool operator==(const Point & o) const
        {
            return node_ == o.node_;
        }
    private:
        Point(lemon::SmartGraph::Node node);
        lemon::SmartGraph::Node node_;
    };

    class Segment final {
    public:
        friend class SteinerTree;
    private:
        Segment(lemon::SmartGraph::Edge edge);
        lemon::SmartGraph::Edge edge_;
    };

    SteinerTree();
    uint32_t numSegments() const;
    uint32_t numPoints() const;
    Point addPoint(const geometry::Point & position);
    Segment addSegment(const Point & p1, const Point & p2);

    Point u(const Segment & segment) const;
    Point v(const Segment & segment) const;
    geometry::Point position(const Point & p) const;
    double length(const Segment& segment) const;
private:
    lemon::SmartGraph graph_;
    lemon::SmartGraph::NodeMap<geometry::Point> position_;
};

}

namespace geometry
{
template <class T>
T make(const interconnection::SteinerTree & tree, const interconnection::SteinerTree::Segment & segment);
}

}



#endif // STEINERTREE_H
