#ifndef STEINERTREE_H
#define STEINERTREE_H

#include <ophidian/geometry/Models.h>
#include <lemon/smart_graph.h>
#include <cstdint>
#include <memory>
#include <iterator>

namespace ophidian {
namespace interconnection {

class SteinerTree
{
public:

    using GraphType = lemon::SmartGraph;

    template <class T>
    class GraphElementWrapper
    {
    public:
        GraphElementWrapper()
        {

        }
        bool operator==(const GraphElementWrapper<T> & o) const
        {
            return el_ == o.el_;
        }
    protected:
        GraphElementWrapper(T element) :
            el_(element)
        {

        }
        T el_;
    };

    class Point final : public GraphElementWrapper<GraphType::Node> {
    public:
        friend class SteinerTree;
        using Parent = GraphElementWrapper<GraphType::Node>;
    private:
        using Parent::Parent;
    };

    class PointSegmentsIterator;
    class Segment final : public GraphElementWrapper<GraphType::Edge> {
    public:
        friend class SteinerTree;
        friend class PointSegmentsIterator;
        using Parent = GraphElementWrapper<GraphType::Edge>;
    private:
        using Parent::Parent;
    };

    class PointIterator {
    public:
        friend class SteinerTree;
        using difference_type = uint32_t;
        using value_type = Point;
        using pointer = const Point*;
        using reference = Point&;
        using iterator_category = std::forward_iterator_tag;
        PointIterator& operator++()
        {
            GraphType::next(point_.el_);
            return *this;
        }
        bool operator==(const PointIterator& o) const
        {
            return point_.el_ == o.point_.el_;
        }
        bool operator!=(const PointIterator& o) const
        {
            return !((*this) == o);
        }
        const Point& operator*() const
        {
            return point_;
        }
    private:
        PointIterator(const Point& p);
        Point point_;
    };

    class PointSegmentsIterator {
    public:
        friend class SteinerTree;
        using difference_type = uint32_t;
        using value_type = Segment;
        using pointer = const Segment*;
        using reference = Segment&;
        using iterator_category = std::forward_iterator_tag;
        const PointSegmentsIterator& operator++()
        {
            ++it_;
            return *this;
        }
        bool operator==(const PointSegmentsIterator& o) const
        {
            return it_ == o.it_;
        }
        bool operator!=(const PointSegmentsIterator& o) const
        {
            return !((*this) == o);
        }
        Segment operator*() const
        {
            return Segment(static_cast<GraphType::Edge>(it_));
        }
    private:
        PointSegmentsIterator(GraphType::IncEdgeIt it);
        GraphType::IncEdgeIt it_;
    };

    class SegmentIterator {
    public:
        friend class SteinerTree;
        using difference_type = uint32_t;
        using value_type = Segment;
        using pointer = const Segment*;
        using reference = Segment&;
        using iterator_category = std::forward_iterator_tag;
        const SegmentIterator& operator++()
        {
            ++it_;
            return *this;
        }
        bool operator==(const SegmentIterator& o) const
        {
            return it_ == o.it_;
        }
        bool operator!=(const SegmentIterator& o) const
        {
            return !((*this) == o);
        }
        Segment operator*() const
        {
            return Segment(static_cast<GraphType::Edge>(it_));
        }
    private:
        SegmentIterator(GraphType::EdgeIt it);
        GraphType::EdgeIt it_;
    };

    SteinerTree();
    Point add(const geometry::Point & position);
    Segment add(const Point & p1, const Point & p2);

    uint32_t size(Segment) const;
    uint32_t size(Point) const;
    Point u(const Segment & segment) const;
    Point v(const Segment & segment) const;
    geometry::Point position(const Point & p) const;
    double length(const Segment& segment) const;
    double length() const;
    std::pair<PointIterator, PointIterator> points() const;
    std::pair<PointSegmentsIterator, PointSegmentsIterator> segments(const Point & point) const;
    std::pair<SegmentIterator, SegmentIterator> segments() const;
private:
    GraphType graph_;
    GraphType::NodeMap<geometry::Point> position_;
    double length_{0.0};
};

}

namespace geometry
{
template <class T>
T make(const interconnection::SteinerTree & tree, const interconnection::SteinerTree::Segment & segment);
}

}



#endif // STEINERTREE_H
