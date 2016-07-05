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

namespace ophidian {
namespace clock_tree_synthesis {
class deffered_merge_embedding
{
public:
    using point = geometry::point<double>;
    using segment = geometry::segment<point>;

    struct trr {
        segment m_core;
        double m_radius;

        trr() {

        }

        trr(segment core, double radius) : m_core(core), m_radius(radius) {

        }

        segment intersection(const trr & other_trr) {
            return segment({0.0, 0.0}, {0.0, 0.0});
        }

        segment intersection(const segment & merging_segment) {
            return segment({0.0, 0.0}, {0.0, 0.0});
        }

        double distance(const point & target_point) {
            return 0.0;
        }
    };

private:
    void build_tree_of_segments(std::vector<clock_topology::node> & sorted_nodes, clock_topology::graph_t::NodeMap<segment> & merging_segments, clock_topology::graph_t::NodeMap<trr> & trrs, clock_topology & clock_topology);
    void find_exact_locations(std::vector<clock_topology::node> & sorted_nodes, clock_topology::graph_t::NodeMap<segment> & merging_segments, clock_topology::graph_t::NodeMap<trr> & trrs, clock_topology & clock_topology);

    std::pair<double, double> calculate_edge_length(clock_topology::node node_a, clock_topology::node node_b);
public:
    deffered_merge_embedding();
    ~deffered_merge_embedding();

    void run_deffered_merge_embedding(clock_topology & clock_topology);
};
}
}

#endif // DEFFERED_MERGE_EMBEDDING_H
