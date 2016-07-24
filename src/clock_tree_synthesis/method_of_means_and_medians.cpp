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


#include "method_of_means_and_medians.h"

namespace ophidian {
namespace clock_tree_synthesis {
method_of_means_and_medians::method_of_means_and_medians()
{

}

method_of_means_and_medians::~method_of_means_and_medians()
{

}

void method_of_means_and_medians::build_topology(method_of_means_and_medians::point clock_source, std::vector<method_of_means_and_medians::point> &flip_flop_positions, clock_topology &clock_topology)
{
    auto source_node = clock_topology.node_create(clock_source);
    build_topology(source_node, flip_flop_positions.begin(), flip_flop_positions.end(), clock_topology, true);
}

method_of_means_and_medians::point method_of_means_and_medians::calculate_center_of_mass(std::vector<point>::iterator positions_begin, std::vector<point>::iterator positions_end)
{
    std::size_t size = std::distance(positions_begin, positions_end);
    point center_of_mass{0.0, 0.0};
    for (auto position = positions_begin; position != positions_end; ++position) {
        center_of_mass.x(center_of_mass.x() + position->x());
        center_of_mass.y(center_of_mass.y() + position->y());
    }
    center_of_mass.x(center_of_mass.x() / (double)size);
    center_of_mass.y(center_of_mass.y() / (double)size);
    return center_of_mass;
}

void method_of_means_and_medians::build_topology(clock_topology::node parent_node, std::vector<point>::iterator positions_begin, std::vector<point>::iterator positions_end, clock_topology &clock_topology, bool sort_by_x_coordinate)
{
    point center_of_mass = calculate_center_of_mass(positions_begin, positions_end);
    auto center_of_mass_node = clock_topology.node_create(center_of_mass);
    clock_topology.edge_create(parent_node, center_of_mass_node);

    auto size = std::distance(positions_begin, positions_end);
    if (size > 1) {
        std::sort(positions_begin, positions_end, coordinate_comparator(sort_by_x_coordinate));
        auto positions_middle = std::next(positions_begin, std::ceil(size / 2.0));
        build_topology(center_of_mass_node, positions_begin, positions_middle, clock_topology, !sort_by_x_coordinate);
        build_topology(center_of_mass_node, positions_middle, positions_end, clock_topology, !sort_by_x_coordinate);
    }
}
}
}


