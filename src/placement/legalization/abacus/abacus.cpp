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
                    std::vector<std::pair<entity_system::entity, double>> sorted_cells;
                    sorted_cells.reserve(m_placement->netlist().cell_count());
                    for (auto cell : m_placement->netlist().cell_system()) {
                        if (!m_placement->cell_fixed(cell)) {
                            sorted_cells.push_back(std::make_pair(cell, m_placement->cell_position(cell).x()));
                        }
                    }
                    std::sort(sorted_cells.begin(), sorted_cells.end(), cell_comparator());

                    for (std::size_t sorted_cell_id = 0; sorted_cell_id < sorted_cells.size(); sorted_cell_id++) {
                        auto cell = sorted_cells.at(sorted_cell_id).first;
                        point cell_position = m_placement->cell_position(cell);
                        point cell_dimensions = m_placement->cell_dimensions(cell);

                        auto abacus_cell = m_cells_system.create();
                        m_cells.netlist_cell(abacus_cell, cell);
                        m_cells.order_id(abacus_cell, sorted_cell_id);
                        m_cells.position(abacus_cell, cell_position);
                        m_cells.width(abacus_cell, cell_dimensions.x());
//                        m_cells.weight(abacus_cell, m_placement->netlist().cell_pins(cell).size());
                        m_cells.weight(abacus_cell, 1);

                        int best_cost = std::numeric_limits<int>::max();
                        int best_y = cell_position.y();
                        unsigned rows_to_search = 3;
                        while (best_cost == std::numeric_limits<int>::max()) {
                            unsigned row_height = cell_dimensions.y();
                            int base_y = std::max(m_floorplan->chip_origin().y(), cell_position.y() - (row_height * (rows_to_search - 1)));
                            int top_y = std::min(m_floorplan->chip_boundaries().y(), cell_position.y() + (row_height * rows_to_search));
                            for (int trial_y = base_y; trial_y < top_y; trial_y += row_height) {
                                point target_position(cell_position.x(), trial_y);
                                m_cells.position(abacus_cell, target_position);
                                try {
                                    entity_system::entity trial_row = m_subrows.find_subrow(target_position);
                                    if (m_abacus_subrows.insert_cell(trial_row, abacus_cell, cell_dimensions.x())) {
                                        place_row(trial_row);
                                        auto last_cell = m_abacus_subrows.cells(trial_row).back();
                                        point last_cell_position = m_cells.position(last_cell);
                                        double cost = std::abs(last_cell_position.x() - cell_position.x()) + std::abs(last_cell_position.y() - cell_position.y());
                                        if (cost < best_cost) {
                                            best_cost = cost;
                                            best_y = trial_y;
                                        }
                                        m_abacus_subrows.remove_last_cell(trial_row, cell_dimensions.x());
                                    }
                                } catch (subrow_not_found) {
                                    continue;
                                }
                            }
                        }

                        point final_position(cell_position.x(), best_y);
                        m_cells.position(abacus_cell, final_position);
                        auto final_row = m_subrows.find_subrow(final_position);
                        if (m_abacus_subrows.insert_cell(final_row, abacus_cell, cell_dimensions.x())) {
                            place_row(final_row);
                        } else {
                            assert(false);
                        }
                    }

                    for (auto subrow : m_subrows_system) {
                        auto & subrow_cells = m_abacus_subrows.cells(subrow);
                        for (auto cell : subrow_cells) {
                            m_placement->cell_position(m_cells.netlist_cell(cell), m_cells.position(cell));
                        }
                    }
                }

                void abacus::place_row(entity_system::entity subrow) {
                    std::list<cluster> clusters;
                    auto cells = m_abacus_subrows.cells(subrow);
                    for (auto cell : cells) {
                        double cell_begin = m_cells.position(cell).x();
                        double cell_width = m_cells.width(cell);
                        auto cluster_it = clusters.end();
                        if (!clusters.empty()) {
                            cluster_it--;
                        }
                        if (clusters.empty() || cluster_it->m_begin + cluster_it->m_size <= cell_begin) {
                            cluster new_cluster(cell_begin);
                            new_cluster.insert_cell(m_cells.order_id(cell), cell_begin, cell_width, m_cells.weight(cell));
                            clusters.insert(clusters.end(), new_cluster);
                        } else {
                            cluster_it->insert_cell(m_cells.order_id(cell), cell_begin, cell_width, m_cells.weight(cell));
                            collapse(clusters, cluster_it, m_subrows.begin(subrow), m_subrows.end(subrow));
                        }
                    }

                    auto & subrow_cells = m_abacus_subrows.cells(subrow);
                    auto cell_it = subrow_cells.begin();
                    for (auto & cluster : clusters) {
                        double x = cluster.m_begin;
                        unsigned i = m_cells.order_id(*cell_it);
                        while (i <= cluster.m_last_order_id) {
                            point cell_position(x, m_cells.position(*cell_it).y());
                            m_cells.position(*cell_it, cell_position);
                            x += m_cells.width(*cell_it);
                            cell_it++;
                            if (cell_it != subrow_cells.end()) {
                                i = m_cells.order_id(*cell_it);
                            } else {
                                i = std::numeric_limits<unsigned>::max();
                            }
                        }
                    }
                }

                void abacus::collapse(std::list<cluster> &clusters, std::list<cluster>::iterator cluster_it, double x_min, double x_max) {
                    bool collapsed = false;
                    while (!collapsed) {
                        double optimal_x = cluster_it->m_displacement / cluster_it->m_weight;
                        optimal_x = std::min(std::max(optimal_x, x_min), x_max);

                        cluster_it->m_begin = optimal_x;
                        if (cluster_it != clusters.begin()) {
                            auto previous_cluster = cluster_it;
                            previous_cluster--;
                            if (previous_cluster->m_begin + previous_cluster->m_size >= cluster_it->m_begin) {
                                previous_cluster->insert_cluster(*cluster_it);
                                clusters.erase(cluster_it);
                                cluster_it = previous_cluster;
                            } else {
                                collapsed = true;
                            }
                        } else {
                            collapsed = true;
                        }
                    }
                }

                void abacus::create_subrows() {
                    for (auto subrow : m_subrows_system) {
                        double subrow_begin = m_subrows.begin(subrow);
                        double subrow_end = m_subrows.end(subrow);
                        m_abacus_subrows.capacity(subrow, subrow_end - subrow_begin + 1);
                    }
                }
            }
        }
    }
}
