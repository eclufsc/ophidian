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

#include "kmeans.h"

namespace ophidian {
namespace register_clustering {

void initialize_centers_from_vector::initialize_centers(entity_system::entity_system & clusters_system, clusters & cluster_properties)
{
    for (auto center : m_initial_centers) {
        auto cluster = clusters_system.create();
        cluster_properties.center(cluster, center);
    }
}

void assign_flip_flop_to_closest_cluster::assign_flops_to_clusters(entity_system::entity_system & clusters_system, clusters & cluster_properties, const std::vector<clusters::cluster_element> &flip_flops)
{
    rtree clusters_rtree;
    for (auto & cluster : clusters_system) {
        clusters_rtree.insert(rtree_node(cluster_properties.center(cluster), cluster));
        cluster_properties.remove_flip_flops(cluster);
    }

    for (auto & flip_flop : flip_flops) {
        std::vector<rtree_node> closest_nodes;
        clusters_rtree.query(boost::geometry::index::nearest(flip_flop.second, 1), std::back_inserter(closest_nodes));
        auto closest_cluster = closest_nodes.front().second;
        cluster_properties.insert_flip_flop(closest_cluster, flip_flop);
    }
}

void update_center_as_mean::update_cluster_centers(entity_system::entity_system &clusters_system, clusters &cluster_properties)
{
    for (auto & cluster : clusters_system) {
        auto flip_flops = cluster_properties.flip_flops(cluster);
        point center_of_mass(0.0, 0.0);
        for (auto & flip_flop : flip_flops) {
            center_of_mass.x(center_of_mass.x() + flip_flop.second.x());
            center_of_mass.y(center_of_mass.y() + flip_flop.second.y());
        }
        center_of_mass.x(center_of_mass.x() / (double)flip_flops.size());
        center_of_mass.y(center_of_mass.y() / (double)flip_flops.size());
        cluster_properties.center(cluster, center_of_mass);
    }
}

}
}

