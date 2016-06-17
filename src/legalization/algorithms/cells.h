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

#ifndef OPHIDIAN_ABACUS_CELLS_H
#define OPHIDIAN_ABACUS_CELLS_H

#include <vector_property.h>
#include "entity_system.h"
#include "../../geometry/geometry.h"

namespace ophidian {
namespace legalization {
namespace abacus {
class cells {
    using point = geometry::point<double>;

    entity_system::entity_system & m_system;

    entity_system::vector_property<entity_system::entity> m_netlist_cells;
    entity_system::vector_property<unsigned> m_order_ids;
    entity_system::vector_property<point> m_positions;
    entity_system::vector_property<double> m_widths;
    entity_system::vector_property<double> m_weights;

public:
    cells(entity_system::entity_system & system);

    entity_system::entity netlist_cell(entity_system::entity cell) const {
        return m_netlist_cells[m_system.lookup(cell)];
    }
    void netlist_cell(entity_system::entity cell, entity_system::entity netlist_cell);

    unsigned order_id(entity_system::entity cell) const {
        return m_order_ids[m_system.lookup(cell)];
    }
    void order_id(entity_system::entity cell, unsigned order_id);

    point position(entity_system::entity cell) const {
        return m_positions[m_system.lookup(cell)];
    }
    void position(entity_system::entity cell, point position);

    double width(entity_system::entity cell) const {
        return m_widths[m_system.lookup(cell)];
    }
    void width(entity_system::entity cell, double width);

    double weight(entity_system::entity cell) const {
        return m_weights[m_system.lookup(cell)];
    }
    void weight(entity_system::entity cell, double weight);
};
}
}
}


#endif //OPHIDIAN_CELLS_H
