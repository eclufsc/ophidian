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

#ifndef ophidian_SITES_H
#define ophidian_SITES_H

#include <vector_property.h>
#include "entity.h"
#include "../geometry/geometry.h"

namespace ophidian {
    namespace floorplan {
        class sites {
            using point = geometry::point<double>;

            entity::system & m_system;

            entity::vector_property<std::string> m_names;
            entity::vector_property<point> m_dimensions;

        public:
            sites(entity::system & system);

            std::string name(entity::entity site) const {
                return m_names[m_system.lookup(site)];
            }

            point dimensions(entity::entity site) const {
                return m_dimensions[m_system.lookup(site)];
            }

            std::pair< std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator > names() const {
                return std::make_pair(m_names.begin(), m_names.end());
            }

            std::pair< std::vector<point>::const_iterator, std::vector<point>::const_iterator > dimensions() const {
                return std::make_pair(m_dimensions.begin(), m_dimensions.end());
            }

            void name(entity::entity site, std::string name);

            void dimensions(entity::entity site, point dimensions);
        };
    }
}



#endif //ophidian_SITES_H
