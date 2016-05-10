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

#ifndef ophidian_DENSITY_H
#define ophidian_DENSITY_H

#include <boost/geometry/index/rtree.hpp>
#include "bins.h"
#include "placement.h"
#include "floorplan.h"

namespace ophidian {


    namespace density {
        class density_map {
            using point = geometry::point<double>;
            using box = geometry::box<point>;
            using rtree_node = std::pair<box, entity_system::entity>;
            using rtree = boost::geometry::index::rtree<rtree_node, boost::geometry::index::rstar<16>>;

            floorplan::floorplan * m_floorplan;
            placement::placement * m_placement;

            entity_system::entity_system m_bins_system;

            bins m_bins;
            rtree m_bins_rtree;
            rtree m_rows_rtree;

            unsigned m_skipped_bins;
        public:

            density_map(floorplan::floorplan * floorplan, placement::placement * placement)
                    : m_floorplan(floorplan), m_placement(placement), m_bins(m_bins_system), m_skipped_bins(0) {
                for (auto row : m_floorplan->rows_system()) {
                    point row_origin = m_floorplan->row_origin(row);
                    point row_dimensions = m_floorplan->row_dimensions(row);
                    box row_box(row_origin, {row_origin.x() + row_dimensions.x(), row_origin.y() + row_dimensions.y()});
                    m_rows_rtree.insert(std::make_pair(row_box, row));
                }
            }

            ~density_map() { }

            entity_system::entity bin_insert(point position, point dimension, double movable_utilization = 0.0, double fixed_utilization = 0.0, double free_space = 0.0);

            std::size_t bin_count() {
                return m_bins_system.size();
            }

            point bin_position(entity_system::entity bin) {
                return m_bins.position(bin);
            }

            point bin_dimension(entity_system::entity bin) {
                return m_bins.dimension(bin);
            }

            double bin_movable_utilization(entity_system::entity bin) {
                return m_bins.movable_utilization(bin);
            }

            double bin_fixed_utilization(entity_system::entity bin) {
                return m_bins.fixed_utilization(bin);
            }

            double bin_free_space(entity_system::entity bin) {
                return m_bins.free_space(bin);
            }

            double bin_area(entity_system::entity bin) {
                double area = m_bins.dimension(bin).x() * m_bins.dimension(bin).y();
                return area;
            }

            void bin_movable_utilization(entity_system::entity bin, double movable_utilization);
            void bin_fixed_utilization(entity_system::entity bin, double fixed_utilization);
            void bin_free_space(entity_system::entity bin, double free_space);

            const bins & bins_properties() const {
                return m_bins;
            }

            const entity_system::entity_system & bins_system() const {
                return m_bins_system;
            }

            void intersecting_bins(box region, std::vector<entity_system::entity> & bins);

            void build_density_map(point max_bin_dimensions, std::vector<double> &utilizations, double bin_area_threshold = 0.2, double free_space_threshold = 0.2);

            unsigned skipped_bins() { return m_skipped_bins; }
        };
    }
}


#endif //ophidian_DENSITY_H
