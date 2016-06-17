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

#ifndef OPHIDIAN_ABACUS_H
#define OPHIDIAN_ABACUS_H

#include "../legalization.h"
#include "cells.h"
#include "subrows.h"

namespace ophidian {
namespace legalization {
namespace abacus {
class cell_comparator {
public:
    bool operator()(const std::pair<entity_system::entity, double> & cell_pair1, const std::pair<entity_system::entity, double> & cell_pair2) {
        return cell_pair1.second < cell_pair2.second;
    }
};

struct cluster {
    double m_begin;
    unsigned m_last_order_id;
    double m_size;
    double m_displacement;
    double m_weight;

    cluster(double begin)
        : m_begin(begin), m_last_order_id(0), m_size(0), m_displacement(0), m_weight(0) {

    }

    void insert_cell(unsigned order_id, double x, double width, double weight) {
        m_last_order_id = order_id;
        m_displacement += weight*(x - m_size);
        m_size += width;
        m_weight += weight;
    }

    void insert_cluster(cluster & cluster) {
        m_last_order_id = cluster.m_last_order_id;
        m_displacement += cluster.m_displacement - (cluster.m_weight - m_size);
        m_size += cluster.m_size;
        m_weight += cluster.m_weight;
    }
};

class abacus : public legalization {
    entity_system::entity_system m_cells_system;
    cells m_cells;

    subrows m_abacus_subrows;

    void align_cells();
    void align_position(point & position);

    void place_row(entity_system::entity subrow);

    void collapse(std::list<cluster> & clusters, std::list<cluster>::iterator cluster_it, double x_min, double x_max, double y);
public:
    abacus(floorplan::floorplan *floorplan, placement::placement *placement)
        : legalization(floorplan, placement), m_cells(m_cells_system), m_abacus_subrows(m_subrows_system) {
        create_subrows();
    }

    void legalize_placement();

    void create_subrows();
};
}
}
}
class abacus {

};


#endif //OPHIDIAN_ABACUS_H
