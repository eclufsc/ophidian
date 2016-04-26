/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#ifndef OPENEDA_DENSITY_H
#define OPENEDA_DENSITY_H

#include <boost/geometry/index/rtree.hpp>
#include "bins.h"
#include "placement.h"
#include "floorplan.h"

namespace ophidian {


    namespace density {
        class density_map {
            using point = geometry::point<double>;
            using box = geometry::box<point>;
            using rtree_node = std::pair<box, entity::entity>;
            using rtree = boost::geometry::index::rtree<rtree_node, boost::geometry::index::rstar<16>>;

            floorplan::floorplan * m_floorplan;
            placement::placement * m_placement;

            entity::system m_bins_system;

            bins m_bins;
            rtree m_bins_rtree;
            rtree m_rows_rtree;

            unsigned m_skipped_bins;
        public:

            density_map(floorplan::floorplan * floorplan, placement::placement * placement)
                    : m_floorplan(floorplan), m_placement(placement), m_bins(m_bins_system), m_skipped_bins(0) {
                for (auto row : m_floorplan->rows_system()) {
                    point row_origin = m_floorplan->row_origin(row.first);
                    point row_dimensions = m_floorplan->row_dimensions(row.first);
                    box row_box(row_origin, {row_origin.x() + row_dimensions.x(), row_origin.y() + row_dimensions.y()});
                    m_rows_rtree.insert(std::make_pair(row_box, row.first));
                }
            }

            ~density_map() { }

            entity::entity bin_insert(point position, point dimension, double movable_utilization = 0.0, double fixed_utilization = 0.0, double free_space = 0.0);

            std::size_t bin_count() {
                return m_bins_system.size();
            }

            point bin_position(entity::entity bin) {
                return m_bins.position(bin);
            }

            point bin_dimension(entity::entity bin) {
                return m_bins.dimension(bin);
            }

            double bin_movable_utilization(entity::entity bin) {
                return m_bins.movable_utilization(bin);
            }

            double bin_fixed_utilization(entity::entity bin) {
                return m_bins.fixed_utilization(bin);
            }

            double bin_free_space(entity::entity bin) {
                return m_bins.free_space(bin);
            }

            double bin_area(entity::entity bin) {
                double area = m_bins.dimension(bin).x() * m_bins.dimension(bin).y();
                return area;
            }

            void bin_movable_utilization(entity::entity bin, double movable_utilization);
            void bin_fixed_utilization(entity::entity bin, double fixed_utilization);
            void bin_free_space(entity::entity bin, double free_space);

            const bins & bins_properties() const {
                return m_bins;
            }

            const entity::system & bins_system() const {
                return m_bins_system;
            }

            void intersecting_bins(box region, std::vector<entity::entity> & bins);

            void build_density_map(point max_bin_dimensions, std::vector<double> &utilizations, double bin_area_threshold = 0.2, double free_space_threshold = 0.2);

            unsigned skipped_bins() { return m_skipped_bins; }
        };
    }
}


#endif //OPENEDA_DENSITY_H
