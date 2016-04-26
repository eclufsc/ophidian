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

#ifndef OPENEDA_BINS_H
#define OPENEDA_BINS_H

#include <vector_property.h>
#include "entity.h"
#include "../geometry/geometry.h"

namespace ophidian {
    namespace density {

        class bins {
            using point = geometry::point<double>;

            entity::system & m_system;

            entity::vector_property<point> m_positions;
            entity::vector_property<point> m_dimensions;
            entity::vector_property<double> m_movable_utilization;
            entity::vector_property<double> m_fixed_utilization;
            entity::vector_property<double> m_free_space;

        public:

            bins(entity::system & system) : m_system(system) {
                m_system.register_property(&m_positions);
                m_system.register_property(&m_dimensions);
                m_system.register_property(&m_movable_utilization);
                m_system.register_property(&m_fixed_utilization);
                m_system.register_property(&m_free_space);
            }

            virtual ~bins() { }

            point position(entity::entity bin) {
                return m_positions[m_system.lookup(bin)];
            }

            point dimension(entity::entity bin) {
                return m_dimensions[m_system.lookup(bin)];
            }

            double movable_utilization(entity::entity bin) {
                return m_movable_utilization[m_system.lookup(bin)];
            }

            double fixed_utilization(entity::entity bin) {
                return m_fixed_utilization[m_system.lookup(bin)];
            }

            double free_space(entity::entity bin) {
                return m_free_space[m_system.lookup(bin)];
            }

            std::pair< std::vector<point>::const_iterator, std::vector<point>::const_iterator > positions() const {
                return std::make_pair(m_positions.begin(), m_positions.end());
            }

            std::pair< std::vector<point>::const_iterator, std::vector<point>::const_iterator > dimensions() const {
                return std::make_pair(m_dimensions.begin(), m_dimensions.end());
            }

            std::pair< std::vector<double>::const_iterator, std::vector<double>::const_iterator > movable_utilizations() const {
                return std::make_pair(m_movable_utilization.begin(), m_movable_utilization.end());
            }

            std::pair< std::vector<double>::const_iterator, std::vector<double>::const_iterator > fixed_utilizations() const {
                return std::make_pair(m_fixed_utilization.begin(), m_fixed_utilization.end());
            }

            std::pair< std::vector<double>::const_iterator, std::vector<double>::const_iterator > free_spaces() const {
                return std::make_pair(m_free_space.begin(), m_free_space.end());
            }

            void position(entity::entity bin, point position);

            void dimension(entity::entity bin, point dimension);

            void movable_utilization(entity::entity bin, double movable_utilization);

            void fixed_utilization(entity::entity bin, double fixed_utilization);

            void free_space(entity::entity bin, double free_space);
        };
    }
}


#endif //OPENEDA_BINS_H
