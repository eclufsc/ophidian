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

#ifndef CLUSTERS_H
#define CLUSTERS_H

#include <unordered_map>
#include "../geometry/geometry.h"
#include "../entity_system/entity_system.h"
#include "../entity_system/vector_property.h"

namespace ophidian {
namespace register_clustering {

class clusters
{
public:
    using point = geometry::point<double>;
    using cluster_element = std::pair<entity_system::entity, point>;

    class cluster_element_comparator {
    private:
        cluster_element m_expected_element;
    public:
        cluster_element_comparator(cluster_element expected_element)
            : m_expected_element(expected_element){

        }

        bool operator()(const cluster_element & element) {
            return (element.first == m_expected_element.first) && boost::geometry::equals(element.second, m_expected_element.second);
        }
    };
private:
    entity_system::entity_system & m_system;

    entity_system::vector_property<std::vector<cluster_element>> m_flip_flops;
    entity_system::vector_property<point> m_centers;

    std::unordered_map<entity_system::entity, entity_system::entity> m_flip_flop_to_cluster;
public:
    clusters(entity_system::entity_system & system);

    void insert_flip_flop(entity_system::entity cluster, cluster_element flip_flop);
    void remove_flip_flop(entity_system::entity cluster, cluster_element flip_flop);
    void remove_flip_flops(entity_system::entity cluster);

    void center(entity_system::entity cluster, point center);

    const std::vector<cluster_element> & flip_flops(entity_system::entity cluster) const {
        return m_flip_flops[m_system.lookup(cluster)];
    }

    std::pair<std::vector<std::vector<cluster_element>>::const_iterator, std::vector<std::vector<cluster_element>>::const_iterator> flip_flops() const {
        return std::make_pair(m_flip_flops.begin(), m_flip_flops.end());
    }

    point center(entity_system::entity cluster) const {
        return m_centers[m_system.lookup(cluster)];
    }

    std::pair<std::vector<point>::const_iterator, std::vector<point>::const_iterator> centers() const {
        return std::make_pair(m_centers.begin(), m_centers.end());
    }

    entity_system::entity cluster(entity_system::entity flip_flop) {
        return m_flip_flop_to_cluster[flip_flop];
    }

};
}
}

#endif // CLUSTERS_H
