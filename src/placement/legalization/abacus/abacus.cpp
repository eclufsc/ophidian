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
                    align_cells();

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
                        m_cells.weight(abacus_cell, std::max((int)m_placement->netlist().cell_pins(cell).size(), 1));

                        int best_cost = std::numeric_limits<int>::max();
                        entity_system::entity best_subrow;
                        unsigned subrows_to_search = 3;
                        while (best_cost == std::numeric_limits<int>::max()) {
                            std::vector<entity_system::entity> closest_subrows = m_subrows.find_closest_subrows(cell_position, subrows_to_search);
                            for (auto trial_subrow : closest_subrows) {
                                double subrow_y = m_floorplan->row_origin(m_subrows.row(trial_subrow)).y();
                                point target_position(cell_position.x(), subrow_y);
                                if (target_position.x() < m_subrows.begin(trial_subrow)) {
                                    target_position.x(m_subrows.begin(trial_subrow));
                                }
                                if (target_position.x() + cell_dimensions.x() - 1 > m_subrows.end(trial_subrow)) {
                                    target_position.x(m_subrows.end(trial_subrow) - cell_dimensions.x() + 1);
                                }
                                m_cells.position(abacus_cell, target_position);
                                if (m_abacus_subrows.insert_cell(trial_subrow, abacus_cell, cell_dimensions.x())) {
                                    place_row(trial_subrow);
                                    auto last_cell = m_abacus_subrows.cells(trial_subrow).back();
                                    point last_cell_position = m_cells.position(last_cell);
                                    double cost = std::abs(last_cell_position.x() - cell_position.x()) + std::abs(last_cell_position.y() - cell_position.y());
                                    if (cost < best_cost) {
                                        best_cost = cost;
                                        best_subrow = trial_subrow;
                                    }
                                    m_abacus_subrows.remove_last_cell(trial_subrow, cell_dimensions.x());
                                }
                            }
                            subrows_to_search *= 2;
                        }

                        double final_y = m_floorplan->row_origin(m_subrows.row(best_subrow)).y();
                        point final_position(cell_position.x(), final_y);
                        if (final_position.x() < m_subrows.begin(best_subrow)) {
                            final_position.x(m_subrows.begin(best_subrow));
                        }
                        if (final_position.x() + cell_dimensions.x() - 1 > m_subrows.end(best_subrow)) {
                            final_position.x(m_subrows.end(best_subrow) - cell_dimensions.x() + 1);
                        }
                        m_cells.position(abacus_cell, final_position);
                        if (m_abacus_subrows.insert_cell(best_subrow, abacus_cell, cell_dimensions.x())) {
                            place_row(best_subrow);
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

                void abacus::align_cells()
                {
                    for (auto cell : m_placement->netlist().cell_system()) {
                        point cell_position = m_placement->cell_position(cell);
                        this->align_position(cell_position);
                        m_placement->cell_position(cell, cell_position);
                    }
                }

                void abacus::align_position(point &position)
                {
                    auto row = m_floorplan->find_closest_row(position);
                    point row_origin = m_floorplan->row_origin(row);
                    double site_width = m_floorplan->site_dimensions(m_floorplan->row_site(row)).x();
                    double aligned_x = std::floor(position.x() / site_width) * site_width;
                    position.x(aligned_x);
                    position.y(row_origin.y());
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
                            collapse(clusters, cluster_it, m_subrows.begin(subrow), m_subrows.end(subrow), m_floorplan->row_origin(m_subrows.row(subrow)).y());
                        }
                    }

                    auto & subrow_cells = m_abacus_subrows.cells(subrow);
                    double subrow_y = m_floorplan->row_origin(m_subrows.row(subrow)).y();
                    auto cell_it = subrow_cells.begin();
                    for (auto & cluster : clusters) {
                        double x = cluster.m_begin;
                        unsigned i = m_cells.order_id(*cell_it);
                        while (i <= cluster.m_last_order_id) {
                            point cell_position(x, subrow_y);
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

                void abacus::collapse(std::list<cluster> &clusters, std::list<cluster>::iterator cluster_it, double x_min, double x_max, double y) {
                    bool collapsed = false;
                    while (!collapsed) {
                        double optimal_x = cluster_it->m_displacement / cluster_it->m_weight;
                        optimal_x = std::min(std::max(optimal_x, x_min), x_max - cluster_it->m_size + 1);
                        point optimal_position(optimal_x, y);
                        this->align_position(optimal_position);

                        cluster_it->m_begin = optimal_position.x();
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
