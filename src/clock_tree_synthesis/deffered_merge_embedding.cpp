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

#include "deffered_merge_embedding.h"

namespace ophidian {
namespace clock_tree_synthesis {

deffered_merge_embedding::deffered_merge_embedding()
{

}

deffered_merge_embedding::~deffered_merge_embedding()
{

}

void deffered_merge_embedding::build_tree_of_segments(std::vector<clock_topology::node> &sorted_nodes, clock_topology::graph_t::NodeMap<segment> &merging_segments, clock_topology::graph_t::NodeMap<trr> & trrs, clock_topology &clock_topology)
{
    for (auto node_it = sorted_nodes.rbegin(); node_it != sorted_nodes.rend(); ++node_it) {
        if (lemon::countOutArcs(clock_topology.graph(), *node_it) == 0) {
            auto node_position = clock_topology.node_position(*node_it);
            merging_segments[*node_it] = segment(node_position, node_position);
        } else {
            std::vector<clock_topology::node> children;
            clock_topology.node_children(*node_it, children);
            assert(children.size() == 2);
            auto node_a = children.front();
            auto node_b = children.back();
            std::pair<double, double> edge_length = calculate_edge_length(node_a, node_b);
            trrs[node_a] = trr(merging_segments[node_a], edge_length.first);
            trrs[node_b] = trr(merging_segments[node_b], edge_length.second);
            merging_segments[*node_it] = trrs[node_a].intersection(trrs[node_b]);
        }
    }
}

void deffered_merge_embedding::find_exact_locations(std::vector<clock_topology::node> & sorted_nodes, const clock_topology::graph_t::NodeMap<segment> &merging_segments, clock_topology &clock_topology)
{
    for (auto node_it = sorted_nodes.begin(); node_it != sorted_nodes.end(); ++node_it) {
        if (lemon::countInArcs(clock_topology.graph(), *node_it) == 0) {
            auto node_position = merging_segments[*node_it].first;
            clock_topology.node_position(*node_it, node_position);
        } else {
            auto parent = clock_topology.node_parent(*node_it);
            auto parent_position = clock_topology.node_position(parent);
            segment parent_merging_segment(parent_position, parent_position);
            double parent_radius = distance(merging_segments[*node_it], parent_position);
            auto trr_parent = trr(parent_merging_segment, parent_radius);
            auto intersection = trr_parent.intersection(merging_segments[*node_it]);
            clock_topology.node_position(*node_it, intersection.first);
        }
    }
}

std::pair<double, double> deffered_merge_embedding::calculate_edge_length(clock_topology::node node_a, clock_topology::node node_b)
{
    return std::make_pair(0.0, 0.0);
}

double deffered_merge_embedding::distance(const deffered_merge_embedding::segment &merging_segment, const deffered_merge_embedding::point &target_point)
{
    return 0.0;
}

void deffered_merge_embedding::run_deffered_merge_embedding(clock_topology &clock_topology)
{
    std::vector<clock_topology::node> sorted_nodes(clock_topology.nodes_count());
    clock_topology::graph_t::NodeMap<unsigned> order(clock_topology.graph());
    lemon::topologicalSort(clock_topology.graph(), order);
    for (auto node_it = clock_topology::graph_t::NodeIt(clock_topology.graph()); node_it != lemon::INVALID; ++node_it) {
        sorted_nodes[order[node_it]] = node_it;
    }

    clock_topology::graph_t::NodeMap<segment> merging_segments(clock_topology.graph());
    clock_topology::graph_t::NodeMap<trr> trrs(clock_topology.graph());
    build_tree_of_segments(sorted_nodes, merging_segments, trrs, clock_topology);
    find_exact_locations(sorted_nodes, merging_segments, clock_topology);
}
}
}

