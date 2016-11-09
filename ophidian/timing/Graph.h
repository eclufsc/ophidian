#ifndef OPHIDIAN_TIMING_GRAPH_H
#define OPHIDIAN_TIMING_GRAPH_H

#include <cstdint>
#include <exception>
#include <lemon/list_graph.h>
#include <lemon/maps.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/util/Range.h>

namespace ophidian {
namespace timing {

enum class Edges {
    kRise,
    kFall,
    kUnlinked
};

class InvalidNode final : public std::exception
{
public:
    const char* what() const noexcept override {
        return "Invalid Node";
    }
};

template <class NodeEntityType>
class Graph final
{
public:
    using GraphType = lemon::ListDigraph;
    using Node = GraphType::Node;
    using Arc = GraphType::Arc;


    //! Node Iterator
    /*!
       The iterator for the Nodes
     */
    class NodeIterator {
    public:
        friend class Graph;

        using difference_type = uint32_t;
        using value_type = Node;
        using pointer = const Node*;
        using reference = Node&;
        using iterator_category = std::forward_iterator_tag;

        inline NodeIterator& operator++() {
            ++node_;
            return *this;
        }
        inline bool operator==(const NodeIterator& o) const {
            return node_ == o.node_;
        }
        inline bool operator!=(const NodeIterator& o) const {
            return !((*this) == o);
        }
        inline Node operator*() const {
            return static_cast<Node>(node_);
        }

    private:
        NodeIterator(const GraphType::NodeIt& node) :
            node_(node)
        {

        }

        GraphType::NodeIt node_;
    };

    class OutArcIterator {
    public:
        friend class Graph;

        using difference_type = uint32_t;
        using value_type = Arc;
        using pointer = const Arc*;
        using reference = Arc&;
        using iterator_category = std::forward_iterator_tag;

        inline OutArcIterator& operator++() {
            ++arc_;
            return *this;
        }
        inline bool operator==(const OutArcIterator& o) const {
            return arc_ == o.arc_;
        }
        inline bool operator!=(const OutArcIterator& o) const {
            return !((*this) == o);
        }
        inline Arc operator*() const {
            return static_cast<Arc>(arc_);
        }

    private:
        OutArcIterator(const GraphType::OutArcIt& arc) :
            arc_(arc)
        {

        }

        GraphType::OutArcIt arc_;
    };

    class InArcIterator {
    public:
        friend class Graph;

        using difference_type = uint32_t;
        using value_type = Arc;
        using pointer = const Arc*;
        using reference = Arc&;
        using iterator_category = std::forward_iterator_tag;

        inline InArcIterator& operator++() {
            ++arc_;
            return *this;
        }
        inline bool operator==(const InArcIterator& o) const {
            return arc_ == o.arc_;
        }
        inline bool operator!=(const InArcIterator& o) const {
            return !((*this) == o);
        }
        inline Arc operator*() const {
            return static_cast<Arc>(arc_);
        }

    private:
        InArcIterator(const GraphType::InArcIt& arc) :
            arc_(arc)
        {

        }

        GraphType::InArcIt arc_;
    };

    class ArcIterator {
    public:
        friend class Graph;

        using difference_type = uint32_t;
        using value_type = Arc;
        using pointer = const Arc*;
        using reference = Arc&;
        using iterator_category = std::forward_iterator_tag;

        inline ArcIterator& operator++() {
            ++arc_;
            return *this;
        }
        inline bool operator==(const ArcIterator& o) const {
            return arc_ == o.arc_;
        }
        inline bool operator!=(const ArcIterator& o) const {
            return !((*this) == o);
        }
        inline Arc operator*() const {
            return static_cast<Arc>(arc_);
        }

    private:
        ArcIterator(const GraphType::ArcIt& arc) :
            arc_(arc)
        {

        }

        GraphType::ArcIt arc_;
    };

    Graph(uint32_t N, const NodeEntityType & defaultNodeEntity) :
        nodeEntities_(graph_),
        nodeEdges_(graph_)
    {
        graph_.reserveNode(N);
        for(uint32_t i = 0; i < N; ++i)
        {
            auto node = graph_.addNode();
            nodeEntities_[node] = defaultNodeEntity;
            nodeEdges_[node] = Edges::kUnlinked;
        }
    }
    ~Graph()
    {

    }
    inline bool empty() const {
        return numNodes() == 0;
    }
    inline uint32_t numNodes() const {
        return lemon::countNodes(graph_);
    }
    Node node(uint32_t id) const  {
        Node n = GraphType::nodeFromId(id);
        if(graph_.valid(n))
        {
            return n;
        }
        throw new InvalidNode;
    }
    uint32_t id(const Node& n) const {
        return graph_.id(n);
    }

    NodeEntityType nodeEntity(const Node& n) const {
        return nodeEntities_[n];
    }

    Edges nodeEdge(const Node& n) const {
        return nodeEdges_[n];
    }

    void linkRiseNode(const Node& n, const NodeEntityType & entity) {
        linkNode(n, entity);
        nodeEdges_[n] = Edges::kRise;
        if(riseMap_)
        {
            (*riseMap_)[entity] = n;
        }
    }

    void linkFallNode(const Node& n, const NodeEntityType & entity) {
        linkNode(n, entity);
        nodeEdges_[n] = Edges::kFall;
        if(fallMap_)
        {
            (*fallMap_)[entity] = n;
        }
    }

    uint32_t numArcs() const {
        return lemon::countArcs(graph_);
    }

    Node source(const Arc& edge) const {
        return graph_.source(edge);
    }

    Node target(const Arc& edge) const {
        return graph_.target(edge);
    }

    Arc connect(const Node& source, const Node& target) {
        return graph_.addArc(source, target);
    }

    template <typename ValueType>
    std::unique_ptr<GraphType::NodeMap<ValueType> > makeProperty(Node) {
        return std::make_unique<GraphType::NodeMap<ValueType> >(graph_);
    }

    void setRiseMap(std::shared_ptr<entity_system::Property<NodeEntityType, GraphType::Node>> map)
    {
        riseMap_ = map;
    }

    void setFallMap(std::shared_ptr<entity_system::Property<NodeEntityType, GraphType::Node>> map)
    {
        fallMap_ = map;
    }


    util::Range<NodeIterator> nodes() const {
        return util::makeRange(begin(Node{}), end(Node{}));
    }


    NodeIterator begin(Node) const {
        return NodeIterator(GraphType::NodeIt(graph_));
    }

    NodeIterator end(Node) const {
        return NodeIterator(lemon::INVALID);
    }

    util::Range<OutArcIterator> outArcs(const Node& node) const {
        return util::makeRange(outArcsBegin(node), outArcsEnd(node));
    }

    OutArcIterator outArcsBegin(const Node& node) const {
        return OutArcIterator(GraphType::OutArcIt(graph_, node));
    }

    OutArcIterator outArcsEnd(const Node& node) const {
        return static_cast<OutArcIterator>(lemon::INVALID);
    }

    util::Range<InArcIterator> inArcs(const Node& node) const {
        return util::makeRange(inArcsBegin(node), inArcsEnd(node));
    }

    InArcIterator inArcsBegin(const Node& node) const {
        return InArcIterator(GraphType::InArcIt(graph_, node));
    }

    InArcIterator inArcsEnd(const Node& node) const {
        return static_cast<InArcIterator>(lemon::INVALID);
    }

private:
    void linkNode(const Node& n, const NodeEntityType& entity)
    {
        nodeEntities_[n] = entity;
    }

    GraphType graph_;
    GraphType::NodeMap<NodeEntityType> nodeEntities_;
    GraphType::NodeMap<Edges> nodeEdges_;
    std::shared_ptr<entity_system::Property<NodeEntityType, GraphType::Node>> riseMap_;
    std::shared_ptr<entity_system::Property<NodeEntityType, GraphType::Node>> fallMap_;

};

}
}

#endif // OPHIDIAN_TIMING_GRAPH_H
