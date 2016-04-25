//
// Created by renan on 3/15/16.
//

#include "bins.h"


namespace openeda {
    namespace density {

        void bins::position(entity::entity bin, bins::point position) {
            m_positions[m_system.lookup(bin)] = position;
        }

        void bins::dimension(entity::entity bin, bins::point dimension) {
            m_dimensions[m_system.lookup(bin)] = dimension;
        }

        void bins::movable_utilization(entity::entity bin, double movable_utilization) {
            m_movable_utilization[m_system.lookup(bin)] = movable_utilization;
        }

        void bins::fixed_utilization(entity::entity bin, double fixed_utilization) {
            m_fixed_utilization[m_system.lookup(bin)] = fixed_utilization;
        }

        void bins::free_space(entity::entity bin, double free_space) {
            m_free_space[m_system.lookup(bin)] = free_space;
        }
    }
}