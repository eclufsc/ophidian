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

#include "sites.h"

namespace ophidian {
    namespace floorplan {

        sites::sites(entity::system &system) : m_system(system) {
            m_system.register_property(&m_names);
            m_system.register_property(&m_dimensions);
        }

        void sites::name(entity::entity site, std::string name) {
            m_names[m_system.lookup(site)] = name;
        }

        void sites::dimensions(entity::entity site, sites::point dimensions) {
            m_dimensions[m_system.lookup(site)] = dimensions;
        }
    }
}
