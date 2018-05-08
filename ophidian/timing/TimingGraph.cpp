#include "TimingGraph.h"

namespace ophidian
{
namespace timing
{

TimingGraph::TimingGraph(const netlist_type & netlist) :
    mPins(mGraph, pin_entity_type()),
    mNodeProperties(mGraph),
    mArcs(mGraph, Arc()),
    mArcProperties(mGraph),
    mRiseNodes(netlist.makeProperty<node_type>(pin_entity_type())),
    mFallNodes(netlist.makeProperty<node_type>(pin_entity_type()))
{

}

TimingGraph::~TimingGraph()
{

}

const TimingGraph::graph_type & TimingGraph::graph() const
{
    return mGraph;
}

size_t TimingGraph::size(node_type) const
{
    return lemon::countNodes(mGraph);
}

size_t TimingGraph::size(arc_type) const
{
    return lemon::countArcs(mGraph);
}


TimingGraph::node_type TimingGraph::nodeCreate(const pin_entity_type & pin, const NodeProperty & prop, entity_system::Property<pin_entity_type, node_type> & map)
{
//    node_type newNode;

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
    node_type newNode = mGraph.addNode();

    // Overrides the node only if the map does not have a valid associated pin or first node on graph.
    if (mGraph.id(map[pin]) >= size(node_type()) || mGraph.id(map[pin]) < 0) {
            map[pin] = newNode;
    }

    mPins[newNode] = pin;
    mNodeProperties[newNode] = prop;

    return newNode;
}

TimingGraph::node_type TimingGraph::riseNodeCreate(const pin_entity_type & pin)
{
    return nodeCreate(pin, NodeProperty::Rise, mRiseNodes);
}

TimingGraph::node_type TimingGraph::riseNode(const pin_entity_type & pin) const
{
    return mRiseNodes[pin];
}

TimingGraph::node_type TimingGraph::fallNodeCreate(const pin_entity_type & pin)
{
    return nodeCreate(pin, NodeProperty::Fall, mFallNodes);
}

TimingGraph::node_type TimingGraph::fallNode(const pin_entity_type & pin) const
{
    return mFallNodes[pin];
}

TimingGraph::arc_type TimingGraph::arcCreate(const node_type & from, const node_type & to, timing_arc_entity_type timingArc)
{
    arc_type graphArc = mGraph.addArc(from, to);
    mArcs[graphArc] = *(static_cast<arc_entity_type*>(&timingArc));
    mArcProperties[graphArc] = ArcProperty::TimingArc;

    return graphArc;
}

TimingGraph::arc_type TimingGraph::arcCreate(const node_type & from, const node_type & to, net_entity_type net)
{
    arc_type graphArc = mGraph.addArc(from, to);
    mArcs[graphArc] = *(static_cast<arc_entity_type*>(&net));
    mArcProperties[graphArc] = ArcProperty::Net;

    return graphArc;
}

TimingGraph::NodeProperty TimingGraph::property(const node_type & node) const
{
    return mNodeProperties[node];
}

TimingGraph::ArcProperty TimingGraph::property(const arc_type & arc) const
{
    return mArcProperties[arc];
}

TimingGraph::node_type TimingGraph::source(const arc_type & arc) const
{
    return mGraph.source(arc);
}

void TimingGraph::source(const arc_type & arc, const node_type & newSource)
{
    mGraph.changeSource(arc, newSource);
}

TimingGraph::node_type TimingGraph::target(const arc_type & arc) const
{
    return mGraph.target(arc);
}

TimingGraph::graph_type::OutArcIt TimingGraph::outArc(const node_type & node) const
{
    return graph_type::OutArcIt(mGraph, node);
}

TimingGraph::graph_type::InArcIt TimingGraph::inArc(const node_type & node) const
{
    return graph_type::InArcIt(mGraph, node);
}

TimingGraph::pin_entity_type TimingGraph::entity(const node_type & node) const
{
    return mPins[node];
}

TimingGraph::net_entity_type TimingGraph::entity(net_entity_type, const arc_type & arc) const
{
    if (mArcProperties[arc] != ArcProperty::Net)
        throw std::logic_error("Incompatible conversion. It is not a net!");

    return mArcs[arc];
}

TimingGraph::timing_arc_entity_type TimingGraph::entity(timing_arc_entity_type, const arc_type & arc) const
{
    if (mArcProperties[arc] != ArcProperty::TimingArc)
        throw std::logic_error("Incompatible conversion. It is not a timing arc!");

    return mArcs[arc];
}

TimingGraph::arc_entity_type TimingGraph::entity(arc_entity_type, const arc_type & arc) const
{
    return mArcs[arc];
}

} // timing
} // ophidian
