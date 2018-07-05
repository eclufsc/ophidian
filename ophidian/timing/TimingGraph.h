/*
 * Copyright 2017 Ophidian
   Licensed to the Apache Software Foundation (ASF) under one
   or more contributor license agreements.  See the NOTICE file
   distributed with this work for additional information
   regarding copyright ownership.  The ASF licenses this file
   to you under the Apache License, Version 2.0 (the
   "License"); you may not use this file except in compliance
   with the License.  You may obtain a copy of the License at
   http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing,
   software distributed under the License is distributed on an
   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
   KIND, either express or implied.  See the License for the
   specific language governing permissions and limitations
   under the License.

 */

#ifndef OPHIDIAN_TIMING_TIMINGGRAPH_H
#define OPHIDIAN_TIMING_TIMINGGRAPH_H

#include <exception>
#include <lemon/list_graph.h>
#include <ophidian/circuit/Netlist.h>
#include <ophidian/timing/TimingArcs.h>
#include <ophidian/timing/Library.h>

namespace ophidian
{
namespace timing
{

//! TimingGraph Class
/*!
   A graph G(V,E), where V is the set of pins and E is the set of nets or timing arcs.
 */
class TimingGraph
{
public:
    using graph_type                       = lemon::ListDigraph;
    using node_type                        = graph_type::Node;
    using arc_type                         = graph_type::Arc;

    template <class T> using node_map_type = graph_type::NodeMap<T>;
    template <class T> using arc_map_type  = graph_type::ArcMap<T>;

    using netlist_type                     = circuit::Netlist;
    using pin_entity_type                  = circuit::Pin;
    using net_entity_type                  = circuit::Net;
    using timing_arc_entity_type           = TimingArcs::timing_arc_entity_type;
    using pin_entity_to_node_property_type = entity_system::Property<pin_entity_type, node_type>;

    enum class NodeProperty
    {
        Rise, Fall
    };

    enum class ArcProperty
    {
        TimingArc, Net
    };

    class Arc : public timing_arc_entity_type, public net_entity_type { };

    using arc_entity_type = Arc;

    //! TimingGraph Constructor
    /*!
       \brief Constructs a TimingGraph with properties from the netlist.
       \param netlist The Netlist of the circuit
     */
    TimingGraph(const netlist_type & netlist);

    TimingGraph(TimingGraph&&) = default;
    TimingGraph& operator=(TimingGraph&&) = default;

    TimingGraph(const TimingGraph&) = delete;
    TimingGraph& operator=(const TimingGraph&) = delete;

    //! Design Destructor
    /*!
       \brief Destroys the TimingGraph.
     */
    ~TimingGraph();

    //! Lemon Graph getter
    /*!
       \brief Get the netlist.
       \return Netlist.
     */
    const graph_type & graph() const;

    //! Size of nodes
    /*!
       \brief Returns the number of Nodes.
       \return The number of nodes.
     */
    size_t size(node_type) const;

    //! Size of edges
    /*!
       \brief Returns the number of Edges.
       \return The number of edges.
     */
    size_t size(arc_type) const;

    //! Create a Rise Node
    /*!
       \brief Creates a node with the Rise property and associates it with a pin.
       \param pin A pin of the circuit.
       \return The node created.
     */
    node_type riseNodeCreate(const pin_entity_type & pin);


    //! Rise Node getter
    /*!
       \brief Take the node associated with a pin.
       \param pin A pin of the circuit.
       \return The node of a pin.
     */
    node_type riseNode(const pin_entity_type & pin) const;

    //! Create a Fall Node
    /*!
       \brief Creates a node with the Fall property and associates it with a pin.
       \param pin A pin of the circuit.
       \return The node created.
     */
    node_type fallNodeCreate(const pin_entity_type & pin);

    //! Fall Node getter const
    /*!
       \brief Take the node associated with a pin.
       \param pin A pin of the circuit.
       \return The node of a pin.
     */
    node_type fallNode(const pin_entity_type & pin) const;

    //! Create a Arc
    /*!
       \brief Creates an arc between two existing nodes.
       \param from The source node.
       \param to The target node.
       \param timingArc The TimingArc entity associated.
       \return The arc created.
     */
    arc_type arcCreate(const node_type & from, const node_type & to, timing_arc_entity_type timingArc);

    //! Create a Arc
    /*!
       \brief Creates an arc between two existing nodes.
       \param from The source node.
       \param to The target node.
       \param net The net entity associated.
       \return The arc created.
     */
    arc_type arcCreate(const node_type & from, const node_type & to, net_entity_type net);

    //! Node Property Getter
    /*!
       \brief Takes the property of a node.
       \param node A node of the graph.
       \return The property of the node.
     */
    NodeProperty property(const node_type & node) const;

    //! Arc Property Getter
    /*!
       \brief Takes the property of a arc.
       \param node A arc of the graph.
       \return The property of the arc.
     */
    ArcProperty property(const arc_type & arc) const;

    //! Source Node Getter
    /*!
       \brief Takes the source node of a arc.
       \param arc A arc of the graph.
       \return The source node.
     */
    node_type source(const arc_type & arc) const;

    //! Change the source node
    /*!
       \brief Change the source node of a arc.
       \param arc A arc of the graph.
       \param newSource The new source.
     */
    void source(const arc_type & arc, const node_type & newSource);

    //! Target Node Getter
    /*!
       \brief Takes the target node of a arc.
       \param arc A arc of the graph.
       \return The target node.
     */
    node_type target(const arc_type & arc) const;

    //! Output edges of a node.
    /*!
       \brief Takes the edges of which the node is the source.
       \param node A node of the graph.
       \return iterator of output edges.
     */
    graph_type::OutArcIt outArc(const node_type & node) const;

    //! Input edges of a node.
    /*!
       \brief Takes the edges of which the node is the target.
       \param node A node of the graph.
       \return iterator of input edges.
     */
    graph_type::InArcIt inArc(const node_type & node) const;

    //! Node's Pin Getter
    /*!
       \brief Returns the pin associated with the node.
       \param node A node of the graph.
       \return A pin of the circuit.
     */
    pin_entity_type entity(const node_type & node) const;

    //! Net's Arc Getter
    /*!
       \brief Returns the net associated with the arc if the property of the arc is Net.
       \param arc A arc of the graph.
       \return A net of the circuit.
     */
    net_entity_type entity(net_entity_type, const arc_type & arc) const;

    //! TimingArc's Arc Getter
    /*!
       \brief Returns the timing arc associated with the arc if the property of the arc is TimingArc.
       \param arc A arc of the graph.
       \return A TimingArc.
     */
    timing_arc_entity_type entity(timing_arc_entity_type, const arc_type & arc) const;

    //! Arc's Arc Getter
    /*!
       \brief Returns the Arc Entity associated with the arc.
       \param arc A arc of the graph.
       \return A Arc.
     */
    arc_entity_type entity(arc_entity_type, const arc_type & arc) const;

    //! Remove Node or Edges
    /*!
       \brief Remove Node or Edges of graph.
       \param begin Iterator of node/arc.
       \param end Iterator of node/arc.
     */
    template <class Iterator>
    void destroy(const Iterator begin, const Iterator end)
    {
        for(Iterator it{begin}; it != end; ++it)
            mGraph.erase(*it);
    }

private:
    //! Create a Node
    /*!
       \brief Creates a generic node.
       \param pin The pin of the circuit.
       \param prop The property of the node.
       \param map The Pin's property of the netlist.
       \return The node created.
     */
    node_type nodeCreate(const pin_entity_type & pin,
                        const NodeProperty & prop,
                        pin_entity_to_node_property_type & map);

    graph_type                       mGraph;
    node_map_type<pin_entity_type>   mPins;
    node_map_type<NodeProperty>      mNodeProperties;
    arc_map_type<arc_entity_type>    mArcs;
    arc_map_type<ArcProperty>        mArcProperties;

    pin_entity_to_node_property_type mRiseNodes;
    pin_entity_to_node_property_type mFallNodes;
};

} // namespace timing
} // namespace ophidian


#endif // OPHIDIAN_TIMING_TIMINGGRAPH_H
