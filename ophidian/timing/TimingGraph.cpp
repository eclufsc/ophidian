#include "TimingGraph.h"

namespace ophidian
{
namespace timing
{

TimingGraph::TimingGraph(const netlist_type & netlist) :
    m_pins(m_graph, pin_entity_type()),
    m_node_properties(m_graph),
    m_arcs(m_graph, Arc()),
    m_arc_properties(m_graph),
    m_rise_nodes(netlist.makeProperty<node_type>(pin_entity_type(), lemon::INVALID)),
    m_fall_nodes(netlist.makeProperty<node_type>(pin_entity_type(), lemon::INVALID))
{

}

TimingGraph::~TimingGraph()
{

}

const TimingGraph::graph_type & TimingGraph::graph() const
{
    return m_graph;
}

size_t TimingGraph::size(node_type) const
{
    return lemon::countNodes(m_graph);
}

size_t TimingGraph::size(arc_type) const
{
    return lemon::countArcs(m_graph);
}

TimingGraph::node_type TimingGraph::node_create(const pin_entity_type & pin, const NodeProperty & prop, pin_entity_to_node_property_type & map)
{
    node_type newNode = m_graph.addNode();

    if (map[pin] == lemon::INVALID) {
        map[pin] = newNode;
    }

    m_pins[newNode] = pin;
    m_node_properties[newNode] = prop;

    return newNode;
}

TimingGraph::node_type TimingGraph::rise_node_create(const pin_entity_type & pin)
{
    return node_create(pin, NodeProperty::Rise, m_rise_nodes);
}

TimingGraph::node_type TimingGraph::rise_node(const pin_entity_type & pin) const
{
    return m_rise_nodes[pin];
}

TimingGraph::node_type TimingGraph::fall_node_create(const pin_entity_type & pin)
{
    return node_create(pin, NodeProperty::Fall, m_fall_nodes);
}

TimingGraph::node_type TimingGraph::fall_node(const pin_entity_type & pin) const
{
    return m_fall_nodes[pin];
}

TimingGraph::arc_type TimingGraph::arc_create(const node_type & from, const node_type & to, timing_arc_entity_type timingArc)
{
    arc_type graphArc = m_graph.addArc(from, to);
    m_arcs[graphArc] = *(static_cast<arc_entity_type*>(&timingArc));
    m_arc_properties[graphArc] = ArcProperty::TimingArc;

    return graphArc;
}

TimingGraph::arc_type TimingGraph::arc_create(const node_type & from, const node_type & to, net_entity_type net)
{
    arc_type graphArc = m_graph.addArc(from, to);
    m_arcs[graphArc] = *(static_cast<arc_entity_type*>(&net));
    m_arc_properties[graphArc] = ArcProperty::Net;

    return graphArc;
}

TimingGraph::NodeProperty TimingGraph::property(const node_type & node) const
{
    return m_node_properties[node];
}

TimingGraph::ArcProperty TimingGraph::property(const arc_type & arc) const
{
    return m_arc_properties[arc];
}

TimingGraph::node_type TimingGraph::source(const arc_type & arc) const
{
    return m_graph.source(arc);
}

void TimingGraph::source(const arc_type & arc, const node_type & newSource)
{
    m_graph.changeSource(arc, newSource);
}

TimingGraph::node_type TimingGraph::target(const arc_type & arc) const
{
    return m_graph.target(arc);
}

TimingGraph::graph_type::OutArcIt TimingGraph::out_arc_iterator(const node_type & node) const
{
    return graph_type::OutArcIt(m_graph, node);
}

TimingGraph::graph_type::InArcIt TimingGraph::in_arc_iterator(const node_type & node) const
{
    return graph_type::InArcIt(m_graph, node);
}

TimingGraph::pin_entity_type TimingGraph::entity(const node_type & node) const
{
    return m_pins[node];
}

TimingGraph::net_entity_type TimingGraph::entity(net_entity_type, const arc_type & arc) const
{
    if (m_arc_properties[arc] != ArcProperty::Net)
        throw std::logic_error("Incompatible conversion. It is not a net!");

    return m_arcs[arc];
}

TimingGraph::timing_arc_entity_type TimingGraph::entity(timing_arc_entity_type, const arc_type & arc) const
{
    if (m_arc_properties[arc] != ArcProperty::TimingArc)
        throw std::logic_error("Incompatible conversion. It is not a timing arc!");

    return m_arcs[arc];
}

TimingGraph::arc_entity_type TimingGraph::entity(arc_entity_type, const arc_type & arc) const
{
    return m_arcs[arc];
}

} // timing
} // ophidian
