#include "TimingGraph.h"

namespace ophidian
{
namespace timing
{


TimingGraph::TimingGraph(const circuit::Netlist & netlist) :
    mPins(mGraph),
    mNodeTypes(mGraph),
    mEdges(mGraph),
    mEdgeTypes(mGraph),
    mRiseNodes(netlist.makeProperty<node_t>(circuit::Pin())),
    mFallNodes(netlist.makeProperty<node_t>(circuit::Pin()))
{
}

TimingGraph::~TimingGraph()
{

}

const TimingGraph::graph_t & TimingGraph::graph()
{
    return mGraph;
}

size_t TimingGraph::nodesAmount()
{
    return lemon::countNodes(mGraph);
}

size_t TimingGraph::edgesAmount()
{
    return lemon::countArcs(mGraph);
}


TimingGraph::node_t TimingGraph::nodeCreate(const circuit::Pin & pin, const node_types & type, entity_system::Property<circuit::Pin, node_t> & map)
{
    node_t newNode;

    if (mGraph.valid(map[pin]))
        if (mPins[map[pin]] == pin)
            newNode = map[pin];
        else
            newNode = mGraph.addNode();
    else
        newNode = mGraph.addNode();

    map[pin] = newNode;
    mPins[newNode] = pin;
    mNodeTypes[newNode] = type;

    return newNode;
}

TimingGraph::node_t TimingGraph::riseNodeCreate(const circuit::Pin & pin)
{
    return nodeCreate(pin, node_types::Rise, mRiseNodes);
}

TimingGraph::node_t TimingGraph::riseNode(const circuit::Pin & pin)
{
    return mRiseNodes[pin];
}

TimingGraph::node_t TimingGraph::fallNodeCreate(const circuit::Pin & pin)
{
    return nodeCreate(pin, node_types::Fall, mFallNodes);
}

TimingGraph::node_t TimingGraph::fallNode(const circuit::Pin & pin)
{
    return mFallNodes[pin];
}

TimingGraph::edge_t TimingGraph::edgeCreate(const node_t & from, const node_t & to, const Edge & edge, const edge_types & type)
{
    edge_t graphEdge = mGraph.addArc(from, to);
    mEdges[graphEdge] = edge;
    mEdgeTypes[graphEdge] = type;

    return graphEdge;
}

node_types TimingGraph::type(const node_t & node)
{
    return mNodeTypes[node];
}

edge_types TimingGraph::type(const edge_t & edge)
{
    return mEdgeTypes[edge];
}

TimingGraph::node_t TimingGraph::source(const edge_t & edge)
{
    return mGraph.source(edge);
}

TimingGraph::node_t TimingGraph::target(const edge_t & edge)
{
    return mGraph.target(edge);
}

TimingGraph::graph_t::OutArcIt TimingGraph::outEdge(const node_t & node)
{
    return graph_t::OutArcIt(mGraph, node);
}

TimingGraph::graph_t::InArcIt TimingGraph::inEdge(const node_t & node)
{
    return graph_t::InArcIt(mGraph, node);
}

circuit::Pin TimingGraph::entity(const node_t & node)
{
    return mPins[node];
}

circuit::Net TimingGraph::entity(circuit::Net, const edge_t & edge)
{
    if (mEdgeTypes[edge] != edge_types::Net)
        throw std::logic_error("Incompatible conversion. It is not a net!");

    return mEdges[edge];
}

Arc TimingGraph::entity(Arc, const edge_t & edge)
{
    if (mEdgeTypes[edge] != edge_types::TimingArc)
        throw std::logic_error("Incompatible conversion. It is not a timing arc!");

    return mEdges[edge];
}

Edge TimingGraph::entity(Edge, const edge_t & edge)
{
    return mEdges[edge];
}

} // timing
} // ophidian
