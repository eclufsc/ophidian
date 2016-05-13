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

#include "density_map.h"

namespace ophidian {
    namespace density {

        entity_system::entity density_map::bin_insert(point position, point dimension, double movable_utilization, double fixed_utilization, double free_space) {
            entity_system::entity bin = m_bins_system.create();
            m_bins.position(bin, position);
            m_bins.dimension(bin, dimension);
            m_bins.movable_utilization(bin, movable_utilization);
            m_bins.fixed_utilization(bin, fixed_utilization);
            m_bins.free_space(bin, free_space);

            box bin_box(position, point(position.x() + dimension.x(), position.y() + dimension.y()));
            rtree_node node(bin_box, bin);
            m_bins_rtree.insert(node);

            return bin;
        }

        void density_map::intersecting_bins(box region, std::vector<entity_system::entity> &bins) {
            std::vector<rtree_node> intersecting_nodes;
            m_bins_rtree.query(boost::geometry::index::intersects(region), std::back_inserter(intersecting_nodes));
            bins.reserve(intersecting_nodes.size());
            for (auto & node : intersecting_nodes) {
                bins.push_back(node.second);
            }
        }

        void density_map::bin_movable_utilization(entity_system::entity bin, double movable_utilization) {
            m_bins.movable_utilization(bin, movable_utilization);
        }

        void density_map::bin_fixed_utilization(entity_system::entity bin, double fixed_utilization) {
            m_bins.fixed_utilization(bin, fixed_utilization);
        }

        void density_map::bin_free_space(entity_system::entity bin, double free_space) {
            m_bins.free_space(bin, free_space);
        }

        void density_map::build_density_map(point max_bin_dimensions, std::vector<double> &utilizations, double bin_area_threshold, double free_space_threshold) {
            std::pair<unsigned, unsigned> number_of_bins(std::ceil(m_floorplan->chip_boundaries().x() / max_bin_dimensions.x()),
                                                         std::ceil(m_floorplan->chip_boundaries().y() / max_bin_dimensions.y()));
            for (unsigned y_index = 0; y_index < number_of_bins.second; y_index++) {
                for (unsigned x_index = 0; x_index < number_of_bins.first; x_index++) {
                    point bin_position(m_floorplan->chip_origin().x() + (x_index * max_bin_dimensions.x()),
                                       m_floorplan->chip_origin().y() + (y_index * max_bin_dimensions.y()));
                    point bin_top(std::min(bin_position.x() + max_bin_dimensions.x(), m_floorplan->chip_boundaries().x()),
                                  std::min(bin_position.y() + max_bin_dimensions.y(), m_floorplan->chip_boundaries().y()));
                    point bin_dimensions(bin_top.x() - bin_position.x(), bin_top.y() - bin_position.y());
                    auto bin = bin_insert(bin_position, bin_dimensions, 0, 0);
                    box bin_region(bin_position, {bin_position.x() + bin_dimensions.x(), bin_position.y() + bin_dimensions.y()});
                    std::vector<entity_system::entity> intersecting_rows;
                    std::vector<rtree_node> intersecting_nodes;
                    m_rows_rtree.query(boost::geometry::index::intersects(bin_region), std::back_inserter(intersecting_nodes));
                    for (auto node : intersecting_nodes) {
                        box row_box = node.first;
                        box intersection;
                        boost::geometry::intersection(bin_region, row_box, intersection);
                        double intersection_area = (intersection.max_corner().x() - intersection.min_corner().x()) * (intersection.max_corner().y() - intersection.min_corner().y());
                        bin_free_space(bin, bin_free_space(bin) + intersection_area);
                    }
                }
            }

            for (auto cell : m_placement->netlist().cell_system()) {
                auto cell_geometry = m_placement->cell_geometry(cell);
                for (auto cell_polygon : cell_geometry) {
                    box cell_rectangle;
                    boost::geometry::envelope(cell_polygon, cell_rectangle);
                    std::vector<entity_system::entity> cell_bins;
                    intersecting_bins(cell_rectangle, cell_bins);

                    for (auto & bin : cell_bins) {
                        point current_bin_position = bin_position(bin);
                        point current_bin_dimension = bin_dimension(bin);
                        box bin_boundaries(current_bin_position, point(current_bin_position.x() + current_bin_dimension.x(), current_bin_position.y() + current_bin_dimension.y()));
                        box intersection;
                        boost::geometry::intersection(bin_boundaries, cell_rectangle, intersection);
                        double intersection_area = (intersection.max_corner().x() - intersection.min_corner().x()) * (intersection.max_corner().y() - intersection.min_corner().y());
                        if (m_placement->cell_fixed(cell)) {
                            bin_fixed_utilization(bin, bin_fixed_utilization(bin) + intersection_area);
                        } else {
                            bin_movable_utilization(bin, bin_movable_utilization(bin) + intersection_area);
                        }
                    }
                }
            }

            utilizations.resize(bin_count());
            for (auto bin : bins_system()) {
                double current_bin_area = bin_area(bin);
                if (current_bin_area > max_bin_dimensions.x() * max_bin_dimensions.y() * bin_area_threshold) {
                    bin_free_space(bin, bin_free_space(bin) - bin_fixed_utilization(bin));
                    double current_bin_free_space = bin_free_space(bin);
                    if (current_bin_free_space > free_space_threshold * current_bin_area) {
                        double utilization = bin_movable_utilization(bin) / current_bin_free_space;
                        utilizations[bin] = utilization;
                    } else {
                        m_skipped_bins++;
                    }
                }
            }
            std::sort(utilizations.begin(), utilizations.end());
        }
    }
}
