/*
 * Copyright 2016 Ophidian
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

#ifndef SRC_TIMING_GRAPH_H_
#define SRC_TIMING_GRAPH_H_

#include <lemon/list_graph.h>
#include "../entity_system/entity_system.h"
#include "transition.h"
#include <unordered_map>

namespace ophidian {
namespace timing {

enum class  edge_types {
    TIMING_ARC, NET
};


struct test {

    lemon::ListDigraph::Node ck;
    lemon::ListDigraph::Node d;
    entity_system::entity tarc;
};

class graph {
public:
    using graph_t = lemon::ListDigraph;
    using node = graph_t::Node;
    using edge = graph_t::Arc;
private:
    graph_t m_graph;

    lemon::ListDigraph::NodeMap< entity_system::entity > m_pins;
    lemon::ListDigraph::NodeMap< edges > m_node_edges;
    lemon::ListDigraph::ArcMap< edge_types > m_arc_types;
    lemon::ListDigraph::ArcMap< entity_system::entity > m_arcs;


    std::unordered_map< entity_system::entity, node > m_rise_nodes;
    std::unordered_map< entity_system::entity, node > m_fall_nodes;

    std::vector< test > m_tests;

    node node_create(entity_system::entity pin, edges node_edge, std::unordered_map<entity_system::entity, node> &map);



public:
    graph();
    virtual ~graph();

    void test_insert(node ck,
                  node d,
                  entity_system::entity tarc);

    const std::vector< test > & tests() const {
        return m_tests;
    }

    const graph_t & G() const {
        return m_graph;
    }


    std::size_t nodes_count() const {
        return lemon::countNodes(m_graph);
    }

    std::size_t edges_count() const {
        return lemon::countArcs(m_graph);
    }

    node rise_node_create(entity_system::entity pin);
    node rise_node(entity_system::entity pin) const {
        return m_rise_nodes.at(pin);
    }

    node fall_node_create(entity_system::entity pin);
    node fall_node(entity_system::entity pin) const {
        return m_fall_nodes.at(pin);
    }

    void node_edge(node u, edges e);
    edges node_edge(node u) const {
        return m_node_edges[u];
    }


    edge edge_create(node u, node v, edge_types type, entity_system::entity entity);
    entity_system::entity edge_entity(edge e) const{
        return m_arcs[e];
    }

    entity_system::entity pin(node u) const {
        return m_pins[u];
    }

    graph_t::OutArcIt out_edges_it(node u) const {
        return graph_t::OutArcIt(m_graph, u);
    }

    node edge_source(edge e)  const {
        return m_graph.source(e);
    }
    node edge_target(edge e)  const {
        return m_graph.target(e);
    }
    edge_types edge_type(edge e) const {
        return m_arc_types[e];
    }

    void edge_source(edge e, node u);


    template <class Iterator>
    void edge_destroy(const Iterator begin, const Iterator end) {
        for(Iterator it{begin}; it != end; ++it)
            m_graph.erase(*it);
    }


};

}
/* namespace timing */
} /* namespace ophidian */

#endif /* SRC_TIMING_GRAPH_H_ */
