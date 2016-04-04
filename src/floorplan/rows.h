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

#ifndef ophidian_ROWS_H
#define ophidian_ROWS_H

#include <vector_property.h>
#include "entity.h"
#include "../geometry/geometry.h"

namespace ophidian {
    namespace floorplan {
        class rows {
            using point = geometry::point<double>;

            entity::system & m_system;

            entity::vector_property<entity::entity> m_sites;
            entity::vector_property<unsigned> m_number_of_sites;
            entity::vector_property<point> m_origins;

        public:
            rows(entity::system & system);

            entity::entity site(entity::entity row) const {
                return m_sites[m_system.lookup(row)];
            }

            unsigned number_of_sites(entity::entity row) const {
                return m_number_of_sites[m_system.lookup(row)];
            }

            point origin(entity::entity row) const {
                return m_origins[m_system.lookup(row)];
            }

            std::pair< std::vector<entity::entity>::const_iterator, std::vector<entity::entity>::const_iterator > sites() const {
                return std::make_pair(m_sites.begin(), m_sites.end());
            }

            std::pair< std::vector<unsigned>::const_iterator, std::vector<unsigned>::const_iterator > number_of_sites() const {
                return std::make_pair(m_number_of_sites.begin(), m_number_of_sites.end());
            }

            std::pair< std::vector<point>::const_iterator, std::vector<point>::const_iterator > origins() const {
                return std::make_pair(m_origins.begin(), m_origins.end());
            }

            void site(entity::entity row, entity::entity site);

            void number_of_sites(entity::entity row, unsigned number_of_sites);

            void origin(entity::entity row, point origin);
        };
    }
}


#endif //ophidian_ROWS_H
