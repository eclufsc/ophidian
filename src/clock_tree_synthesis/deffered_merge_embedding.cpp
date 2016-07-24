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

deffered_merge_embedding::deffered_merge_embedding(double resistance_per_unit, double capacitance_per_unit)
    : m_resistance_per_unit(resistance_per_unit), m_capacitance_per_unit(capacitance_per_unit) {

}

deffered_merge_embedding::~deffered_merge_embedding()
{

}

void deffered_merge_embedding::build_tree_of_segments(std::vector<clock_topology::node> &sorted_nodes, clock_topology::graph_t::NodeMap<segment> &merging_segments, clock_topology::graph_t::NodeMap<trr> & trrs, clock_topology &topology)
{
    for (auto node_it = sorted_nodes.rbegin(); node_it != sorted_nodes.rend(); ++node_it) {
        if (lemon::countOutArcs(topology.graph(), *node_it) == 0) {
            auto node_position = topology.node_position(*node_it);
            merging_segments[*node_it] = segment(node_position, node_position);
        } else {
            std::vector<clock_topology::node> children;
            topology.node_children(*node_it, children);
            assert(children.size() == 2);
            auto node_a = children.front();
            auto node_b = children.back();
            std::pair<double, double> edge_length = calculate_edge_length(node_a, node_b, topology);
            trrs[node_a] = trr(merging_segments[node_a], edge_length.first);
            trrs[node_b] = trr(merging_segments[node_b], edge_length.second);
            merging_segments[*node_it] = trrs[node_a].intersection(trrs[node_b]);
            update_node_delay_and_capacitance(*node_it, merging_segments, topology);
        }
    }
}

void deffered_merge_embedding::find_exact_locations(std::vector<clock_topology::node> & sorted_nodes, const clock_topology::graph_t::NodeMap<segment> &merging_segments, clock_topology &topology)
{
    for (auto node_it = sorted_nodes.begin(); node_it != sorted_nodes.end(); ++node_it) {
        if (lemon::countInArcs(topology.graph(), *node_it) == 0) {
            auto node_position = merging_segments[*node_it].first;
            topology.node_position(*node_it, node_position);
        } else {
            auto parent = topology.node_parent(*node_it);
            auto parent_position = topology.node_position(parent);
            segment parent_merging_segment(parent_position, parent_position);
            double parent_radius = distance(merging_segments[*node_it], parent_position);
            auto trr_parent = trr(parent_merging_segment, parent_radius);
            auto intersection = trr_parent.intersection(merging_segments[*node_it]);
            topology.node_position(*node_it, intersection.first);
        }
    }
}

void deffered_merge_embedding::update_node_delay_and_capacitance(clock_topology::node node, clock_topology::graph_t::NodeMap<deffered_merge_embedding::segment> &merging_segments, clock_topology & topology)
{
    if (lemon::countOutArcs(topology.graph(), node) == 0) {
        topology.node_delay(node, 0.0);
        topology.node_capacitance(node, 1.0);
    } else {
        double max_delay = std::numeric_limits<double>::min();
        double capacitance = 0.0;
        for (auto arc_it = clock_topology::graph_t::ArcIt(topology.graph()); arc_it != lemon::INVALID; ++arc_it) {
            auto child_node = topology.graph().target(arc_it);
            double child_delay = topology.node_delay(child_node);
            double child_capacitance = topology.node_capacitance(child_node);
            double distance_to_child = distance(merging_segments[node], merging_segments[child_node]);
            double node_delay = m_resistance_per_unit*distance_to_child*(m_capacitance_per_unit*distance_to_child/2.0 + child_capacitance) + child_delay;
            max_delay = std::max(max_delay, node_delay);
            capacitance += child_capacitance;
        }
        topology.node_delay(node, max_delay);
        topology.node_capacitance(node, capacitance);
    }
}

std::pair<double, double> deffered_merge_embedding::calculate_edge_length(clock_topology::node node_a, clock_topology::node node_b, clock_topology & topology)
{
    auto node_a_position = topology.node_position(node_a);
    auto node_b_position = topology.node_position(node_b);
    double distance = std::abs(node_a_position.x() - node_b_position.x()) + std::abs(node_a_position.y() - node_b_position.y());
    double t1 = topology.node_delay(node_a);
    double t2 = topology.node_delay(node_b);
    double c1 = topology.node_capacitance(node_a);
    double c2 = topology.node_capacitance(node_b);
    double merging_point = (t2 - t1 + m_resistance_per_unit*distance*(c2 + ((m_capacitance_per_unit*distance)/2.0)))/(m_resistance_per_unit*(c1 + c2 + m_capacitance_per_unit*distance));
    if (merging_point >= 0 && merging_point <= distance) {
        return std::make_pair(merging_point, distance - merging_point);
    }
    if (merging_point < 0) {
        double extended_distance = (std::sqrt((m_resistance_per_unit*c2*m_resistance_per_unit*c2) + (2*m_resistance_per_unit*m_capacitance_per_unit*(t1 - t2))) - (m_resistance_per_unit*c2))/(m_resistance_per_unit*m_capacitance_per_unit);
        return std::make_pair(0.0, extended_distance);
    } else {
        double extended_distance = (std::sqrt((m_resistance_per_unit*c1*m_resistance_per_unit*c1) + (2*m_resistance_per_unit*m_capacitance_per_unit*(t2 - t1))) - (m_resistance_per_unit*c1))/(m_resistance_per_unit*m_capacitance_per_unit);
        return std::make_pair(extended_distance, 0.0);
    }
}

double deffered_merge_embedding::distance(const deffered_merge_embedding::segment &merging_segment, const deffered_merge_embedding::point &target_point)
{
    double distance1 = std::abs(merging_segment.first.x() - target_point.x()) + std::abs(merging_segment.first.y() - target_point.y());
    double distance2 = std::abs(merging_segment.second.x() - target_point.x()) + std::abs(merging_segment.second.y() - target_point.y());
    return std::min(distance1, distance2);
}

double deffered_merge_embedding::distance(const deffered_merge_embedding::segment & merging_segment1, const deffered_merge_embedding::segment & merging_segment2)
{
    double distance1 = distance(merging_segment1, merging_segment2.first);
    double distance2 = distance(merging_segment1, merging_segment2.second);
    return std::min(distance1, distance2);
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

