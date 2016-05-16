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
#include "entity.h"
#include "../../../geometry/geometry.h"

namespace ophidian {
    namespace placement {
        namespace legalization {
            namespace abacus {
                class cells {
                    using point = geometry::point<double>;

                    entity::system & m_system;

                    entity::vector_property<entity::entity> m_netlist_cells;
                    entity::vector_property<unsigned> m_order_ids;
                    entity::vector_property<point> m_positions;
                    entity::vector_property<double> m_widths;
                    entity::vector_property<double> m_weights;

                public:
                    cells(entity::system & system);

                    entity::entity netlist_cell(entity::entity cell) const {
                        return m_netlist_cells[m_system.lookup(cell)];
                    }
                    void netlist_cell(entity::entity cell, entity::entity netlist_cell);

                    unsigned order_id(entity::entity cell) const {
                        return m_order_ids[m_system.lookup(cell)];
                    }
                    void order_id(entity::entity cell, unsigned order_id);

                    point position(entity::entity cell) const {
                        return m_positions[m_system.lookup(cell)];
                    }
                    void position(entity::entity cell, point position);

                    double width(entity::entity cell) const {
                        return m_widths[m_system.lookup(cell)];
                    }
                    void width(entity::entity cell, double width);

                    double weight(entity::entity cell) const {
                        return m_weights[m_system.lookup(cell)];
                    }
                    void weight(entity::entity cell, double weight);
                };
            }
        }
    }
}


#endif //OPHIDIAN_ABACUS_CELLS_H
