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

#include "cells.h"

namespace ophidian {
    namespace placement {
        namespace legalization {
            namespace abacus {

                cells::cells(entity_system::entity_system &system)
                    : m_system(system) {
                    m_system.register_property(&m_netlist_cells);
                    m_system.register_property(&m_order_ids);
                    m_system.register_property(&m_positions);
                    m_system.register_property(&m_widths);
                    m_system.register_property(&m_weights);
                }

                void cells::netlist_cell(entity_system::entity cell, entity_system::entity netlist_cell) {
                    m_netlist_cells[m_system.lookup(cell)] = netlist_cell;
                }

                void cells::order_id(entity_system::entity cell, unsigned order_id) {
                    m_order_ids[m_system.lookup(cell)] = order_id;
                }

                void cells::position(entity_system::entity cell, cells::point position) {
                    m_positions[m_system.lookup(cell)] = position;
                }

                void cells::width(entity_system::entity cell, double width) {
                    m_widths[m_system.lookup(cell)] = width;
                }

                void cells::weight(entity_system::entity cell, double weight) {
                    m_weights[m_system.lookup(cell)] = weight;
                }
            }
        }
    }
}
