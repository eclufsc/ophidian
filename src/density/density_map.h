//
// Created by renan on 3/15/16.
//

#ifndef OPENEDA_DENSITY_H
#define OPENEDA_DENSITY_H

#include <boost/geometry/index/rtree.hpp>
#include "bins.h"

namespace openeda {
    namespace density {
        class density_map {
            using point = geometry::point<double>;
            using box = geometry::box<point>;
            using rtree_node = std::pair<box, entity::entity>;
            using rtree = boost::geometry::index::rtree<rtree_node, boost::geometry::index::rstar<16>>;

            entity::system m_bins_system;

            bins m_bins;
            rtree m_bins_rtree;


        public:
            density_map() : m_bins(m_bins_system) {

            }

            ~density_map() { }

            entity::entity bin_insert(point position, point dimension, double movable_utilization, double fixed_utilization);

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
                double bin_area = m_bins.dimension(bin).x() * m_bins.dimension(bin).y();
                double free_space = bin_area - m_bins.movable_utilization(bin);
                return free_space;
            }

            void bin_movable_utilization(entity::entity bin, double movable_utilization);

            const bins & bins_properties() const {
                return m_bins;
            }

            const entity::system & bins_system() const {
                return m_bins_system;
            }

            entity::entity intersecting_bins(box region, std::vector<entity::entity> & bins);
        };
    }
}


#endif //OPENEDA_DENSITY_H
