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

#ifndef DEFFERED_MERGE_EMBEDDING_H
#define DEFFERED_MERGE_EMBEDDING_H

#include "clock_topology.h"
#include "../geometry/geometry.h"
#include "lemon/connectivity.h"
#include "boost/geometry.hpp"

namespace ophidian {
namespace clock_tree_synthesis {
class deffered_merge_embedding
{
public:
    using point = geometry::point<double>;
    using segment = geometry::segment<point>;
    using box = geometry::box<point>;

    struct trr {
    private:
        segment m_core;
        double m_radius;
        box m_box;
    public:
        trr() {

        }

        trr(segment core, double radius) : m_core(core), m_radius(radius) {
            point min_corner(std::min(core.first.x() - radius, core.second.x() - radius), std::min(core.first.y() - radius, core.second.y() - radius));
            point max_corner(std::max(core.first.x() + radius, core.second.x() + radius), std::max(core.first.y() + radius, core.second.y() + radius));
            point rotated_min_corner;
            geometry::rotate(min_corner, 45.0, rotated_min_corner);
            point rotated_max_corner;
            geometry::rotate(max_corner, 45.0, rotated_max_corner);
            m_box = box(rotated_min_corner, rotated_max_corner);
        }

        segment intersection(const trr & other_trr) {
            box intersection_box;
            boost::geometry::intersection(m_box, other_trr.m_box, intersection_box);
            segment merge_segment(intersection_box.min_corner(), intersection_box.max_corner());
            segment rotated_merge_segment;
            geometry::rotate(merge_segment, -45.0, rotated_merge_segment);
            return rotated_merge_segment;
        }

        segment intersection(const segment & merging_segment) {
            trr other_trr(merging_segment, 0);
            return intersection(other_trr);
        }
    };

private:
    void build_tree_of_segments(std::vector<clock_topology::node> & sorted_nodes, clock_topology::graph_t::NodeMap<segment> & merging_segments, clock_topology::graph_t::NodeMap<trr> & trrs, clock_topology & clock_topology);
    void find_exact_locations(std::vector<clock_topology::node> & sorted_nodes, const clock_topology::graph_t::NodeMap<segment> &merging_segments, clock_topology & clock_topology);

    std::pair<double, double> calculate_edge_length(clock_topology::node node_a, clock_topology::node node_b);
    double distance(const segment & merging_segment, const point & target_point);
public:
    deffered_merge_embedding();
    ~deffered_merge_embedding();

    void run_deffered_merge_embedding(clock_topology & clock_topology);
};
}
}

#endif // DEFFERED_MERGE_EMBEDDING_H
