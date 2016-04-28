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

#include "abacus.h"

namespace ophidian {
    namespace placement {
        namespace legalization {
            namespace abacus {

                void abacus::legalize_placement() {
                    std::vector<entity::entity> sorted_cells;
                    sorted_cells.reserve(m_placement->netlist().cell_count());
                    for (auto cell : m_placement->netlist().cell_system()) {
                        if (!m_placement->cell_fixed(cell.first)) {
                            sorted_cells.push_back(cell.first);
                        }
                    }
                    std::sort(sorted_cells.begin(), sorted_cells.end(), cell_comparator(m_placement));

                    for (std::size_t sorted_cell_id = 0; sorted_cell_id < sorted_cells.size(); sorted_cell_id++) {
                        auto cell = sorted_cells.at(sorted_cell_id);
                        point cell_position = m_placement->cell_position(cell);

                        auto abacus_cell = m_cells_system.create();
                        m_cells.netlist_cell(abacus_cell, cell);
                        m_cells.order_id(abacus_cell, sorted_cell_id);
                        m_cells.position(abacus_cell, cell_position);
                        m_cells.weight(abacus_cell, m_placement->netlist().cell_pins(cell).size());

                        int best_cost = std::numeric_limits<int>::max();
                        int best_y = cell_position.y();
                        unsigned rows_to_search = 3;
                        while (best_cost == std::numeric_limits<int>::max()) {
                            unsigned row_height = 3420;
                            int base_y = std::max(m_floorplan->chip_origin().y(), cell_position.y() - (row_height * (rows_to_search - 1)));
                            int top_y = std::min(m_floorplan->chip_boundaries().y(), cell_position.y() + (row_height * rows_to_search));
                            for (int trial_y = base_y; trial_y < top_y; trial_y += row_height) {
                                point target_position(cell_position.x(), trial_y);
                                m_cells.position(abacus_cell, target_position);
                                entity::entity trial_row = m_subrows.find_subrow(target_position);
                                // insert cell in row
                                // update best cost
                            }
                        }
                    }
                }
            }
        }
    }
}
