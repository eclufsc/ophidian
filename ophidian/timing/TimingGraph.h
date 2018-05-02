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
    enum class NodeProperty
    {
        Rise, Fall
    };

    enum class ArcProperty
    {
        TimingArc, Net
    };

    class Arc : public TimingArc, public circuit::Net { };

    using GraphType = lemon::ListDigraph;
    using NodeType = GraphType::Node;
    using ArcType = GraphType::Arc;

    //! TimingGraph Constructor
    /*!
       \brief Constructs a TimingGraph with properties from the netlist.
       \param netlist The Netlist of the circuit
     */
    TimingGraph(const circuit::Netlist & netlist);

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
    const GraphType & graph() const;

    //! Size of nodes
    /*!
       \brief Returns the number of Nodes.
       \return The number of nodes.
     */
    size_t size(NodeType) const;

    //! Size of edges
    /*!
       \brief Returns the number of Edges.
       \return The number of edges.
     */
    size_t size(ArcType) const;

    //! Create a Rise Node
    /*!
       \brief Creates a node with the Rise property and associates it with a pin.
       \param pin A pin of the circuit.
       \return The node created.
     */
    NodeType riseNodeCreate(const circuit::Pin & pin);


    //! Rise Node getter
    /*!
       \brief Take the node associated with a pin.
       \param pin A pin of the circuit.
       \return The node of a pin.
     */
    NodeType riseNode(const circuit::Pin & pin) const;

    //! Create a Fall Node
    /*!
       \brief Creates a node with the Fall property and associates it with a pin.
       \param pin A pin of the circuit.
       \return The node created.
     */
    NodeType fallNodeCreate(const circuit::Pin & pin);

    //! Fall Node getter const
    /*!
       \brief Take the node associated with a pin.
       \param pin A pin of the circuit.
       \return The node of a pin.
     */
    NodeType fallNode(const circuit::Pin & pin) const;

    //! Create a Arc
    /*!
       \brief Creates an arc between two existing nodes.
       \param from The source node.
       \param to The target node.
       \param timingArc The TimingArc entity associated.
       \return The arc created.
     */
    ArcType arcCreate(const NodeType & from, const NodeType & to, TimingArc timingArc);

    //! Create a Arc
    /*!
       \brief Creates an arc between two existing nodes.
       \param from The source node.
       \param to The target node.
       \param net The net entity associated.
       \return The arc created.
     */
    ArcType arcCreate(const NodeType & from, const NodeType & to, circuit::Net net);

    //! Node Property Getter
    /*!
       \brief Takes the property of a node.
       \param node A node of the graph.
       \return The property of the node.
     */
    NodeProperty property(const NodeType & node) const;

    //! Arc Property Getter
    /*!
       \brief Takes the property of a arc.
       \param node A arc of the graph.
       \return The property of the arc.
     */
    ArcProperty property(const ArcType & arc) const;

    //! Source Node Getter
    /*!
       \brief Takes the source node of a arc.
       \param arc A arc of the graph.
       \return The source node.
     */
    NodeType source(const ArcType & arc) const;

    //! Change the source node
    /*!
       \brief Change the source node of a arc.
       \param arc A arc of the graph.
       \param newSource The new source.
     */
    void source(const ArcType & arc, const NodeType & newSource);

    //! Target Node Getter
    /*!
       \brief Takes the target node of a arc.
       \param arc A arc of the graph.
       \return The target node.
     */
    NodeType target(const ArcType & arc) const;

    //! Output edges of a node.
    /*!
       \brief Takes the edges of which the node is the source.
       \param node A node of the graph.
       \return iterator of output edges.
     */
    GraphType::OutArcIt outArc(const NodeType & node) const;

    //! Input edges of a node.
    /*!
       \brief Takes the edges of which the node is the target.
       \param node A node of the graph.
       \return iterator of input edges.
     */
    GraphType::InArcIt inArc(const NodeType & node) const;

    //! Node's Pin Getter
    /*!
       \brief Returns the pin associated with the node.
       \param node A node of the graph.
       \return A pin of the circuit.
     */
    circuit::Pin entity(const NodeType & node) const;

    //! Net's Arc Getter
    /*!
       \brief Returns the net associated with the arc if the property of the arc is Net.
       \param arc A arc of the graph.
       \return A net of the circuit.
     */
    circuit::Net entity(circuit::Net, const ArcType & arc) const;

    //! TimingArc's Arc Getter
    /*!
       \brief Returns the timing arc associated with the arc if the property of the arc is TimingArc.
       \param arc A arc of the graph.
       \return A TimingArc.
     */
    TimingArc entity(TimingArc, const ArcType & arc) const;

    //! Arc's Arc Getter
    /*!
       \brief Returns the Arc Entity associated with the arc.
       \param arc A arc of the graph.
       \return A Arc.
     */
    Arc entity(Arc, const ArcType & arc) const;

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
    NodeType nodeCreate(const circuit::Pin & pin,
                        const NodeProperty & prop,
                        entity_system::Property<circuit::Pin, NodeType> & map);

    GraphType                        mGraph;
    GraphType::NodeMap<circuit::Pin> mPins;
    GraphType::NodeMap<NodeProperty> mNodeProperties;
    GraphType::ArcMap<Arc>           mArcs;
    GraphType::ArcMap<ArcProperty>   mArcProperties;

    entity_system::Property<circuit::Pin, NodeType> mRiseNodes;
    entity_system::Property<circuit::Pin, NodeType> mFallNodes;
};

} // namespace timing
} // namespace ophidian


#endif // OPHIDIAN_TIMING_TIMINGGRAPH_H