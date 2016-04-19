//
// Created by renan on 3/15/16.
//

#include "density_map.h"

namespace openeda {
    namespace density {

        entity::entity density_map::bin_insert(density_map::point position, density_map::point dimension, double movable_utilization, double fixed_utilization) {
            entity::entity bin = m_bins_system.create();
            m_bins.position(bin, position);
            m_bins.dimension(bin, dimension);
            m_bins.movable_utilization(bin, movable_utilization);
            m_bins.fixed_utilization(bin, fixed_utilization);

            box bin_box(position, point(position.x() + dimension.x(), position.y() + dimension.y()));
            m_bins_rtree.insert(std::make_pair(bin_box, bin));
        }

        void density_map::intersecting_bins(density_map::box region, std::vector<entity::entity> &bins) {
            std::vector<rtree_node> intersecting_nodes;
            m_bins_rtree.query(boost::geometry::index::intersects(region), intersecting_nodes);
            bins.reserve(intersecting_nodes.size());
            for (auto & node : intersecting_nodes) {
                bins.push_back(node.second);
            }
        }

        void density_map::add_movable_utilization(entity::entity bin, double movable_utilization) {
            auto current_movable_utilization = m_bins.movable_utilization(bin);
            m_bins.movable_utilization(bin, current_movable_utilization + movable_utilization);
        }
    }
}
