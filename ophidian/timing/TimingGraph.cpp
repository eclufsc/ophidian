#include "TimingGraph.h"

namespace ophidian
{
namespace timing
{

TimingGraph::TimingGraph(const circuit::Netlist & netlist) :
    mPins(mGraph, circuit::Pin()),
    mNodeProperties(mGraph),
    mArcs(mGraph, Arc()),
    mArcProperties(mGraph),
    mRiseNodes(netlist.makeProperty<NodeType>(circuit::Pin())),
    mFallNodes(netlist.makeProperty<NodeType>(circuit::Pin()))
{

}

TimingGraph::~TimingGraph()
{

}

const TimingGraph::graph_type & TimingGraph::graph() const
{
    return mGraph;
}

size_t TimingGraph::size(NodeType) const
{
    return lemon::countNodes(mGraph);
}

size_t TimingGraph::size(ArcType) const
{
    return lemon::countArcs(mGraph);
}


TimingGraph::NodeType TimingGraph::nodeCreate(const circuit::Pin & pin, const NodeProperty & prop, entity_system::Property<circuit::Pin, NodeType> & map)
{
//    NodeType newNode;

//    if (mGraph.valid(map[pin]))
//        if (mPins[map[pin]] == pin)
//            newNode = map[pin];
//        else
//            newNode = mGraph.addNode();
//    else
//        newNode = mGraph.addNode();

//    map[pin] = newNode;
//    mPins[newNode] = pin;
//    mNodeProperties[newNode] = prop;


    // Algorithm uses more than one node per pin?
    // It changes the source of the input arcs of the circuit.
    // You need to create tests to see if it works correctly.
    NodeType newNode = mGraph.addNode();

    // Overrides the node only if the map does not have a valid associated pin or first node on graph.
    if (mGraph.id(map[pin]) >= size(NodeType()) || mGraph.id(map[pin]) < 0) {
            map[pin] = newNode;
    }

    mPins[newNode] = pin;
    mNodeProperties[newNode] = prop;

    return newNode;
}

TimingGraph::NodeType TimingGraph::riseNodeCreate(const circuit::Pin & pin)
{
    return nodeCreate(pin, NodeProperty::Rise, mRiseNodes);
}

TimingGraph::NodeType TimingGraph::riseNode(const circuit::Pin & pin) const
{
    return mRiseNodes[pin];
}

TimingGraph::NodeType TimingGraph::fallNodeCreate(const circuit::Pin & pin)
{
    return nodeCreate(pin, NodeProperty::Fall, mFallNodes);
}

TimingGraph::NodeType TimingGraph::fallNode(const circuit::Pin & pin) const
{
    return mFallNodes[pin];
}

TimingGraph::ArcType TimingGraph::arcCreate(const NodeType & from, const NodeType & to, TimingArc timingArc)
{
    ArcType graphArc = mGraph.addArc(from, to);
    mArcs[graphArc] = *(static_cast<Arc*>(&timingArc));
    mArcProperties[graphArc] = ArcProperty::TimingArc;

    return graphArc;
}

TimingGraph::ArcType TimingGraph::arcCreate(const NodeType & from, const NodeType & to, circuit::Net net)
{
    ArcType graphArc = mGraph.addArc(from, to);
    mArcs[graphArc] = *(static_cast<Arc*>(&net));
    mArcProperties[graphArc] = ArcProperty::Net;

    return graphArc;
}

TimingGraph::NodeProperty TimingGraph::property(const NodeType & node) const
{
    return mNodeProperties[node];
}

TimingGraph::ArcProperty TimingGraph::property(const ArcType & arc) const
{
    return mArcProperties[arc];
}

TimingGraph::NodeType TimingGraph::source(const ArcType & arc) const
{
    return mGraph.source(arc);
}

void TimingGraph::source(const ArcType & arc, const NodeType & newSource)
{
    mGraph.changeSource(arc, newSource);
}

TimingGraph::NodeType TimingGraph::target(const ArcType & arc) const
{
    return mGraph.target(arc);
}

TimingGraph::graph_type::OutArcIt TimingGraph::outArc(const NodeType & node) const
{
    return graph_type::OutArcIt(mGraph, node);
}

TimingGraph::graph_type::InArcIt TimingGraph::inArc(const NodeType & node) const
{
    return graph_type::InArcIt(mGraph, node);
}

circuit::Pin TimingGraph::entity(const NodeType & node) const
{
    return mPins[node];
}

circuit::Net TimingGraph::entity(circuit::Net, const ArcType & arc) const
{
    if (mArcProperties[arc] != ArcProperty::Net)
        throw std::logic_error("Incompatible conversion. It is not a net!");

    return mArcs[arc];
}

TimingArc TimingGraph::entity(TimingArc, const ArcType & arc) const
{
    if (mArcProperties[arc] != ArcProperty::TimingArc)
        throw std::logic_error("Incompatible conversion. It is not a timing arc!");

    return mArcs[arc];
}

TimingGraph::Arc TimingGraph::entity(Arc, const ArcType & arc) const
{
    return mArcs[arc];
}

} // timing
} // ophidian
