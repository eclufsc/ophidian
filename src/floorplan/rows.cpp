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

#include "rows.h"

namespace ophidian {
    namespace floorplan {

        rows::rows(entity::system &system) : m_system(system) {
            m_system.register_property(&m_sites);
            m_system.register_property(&m_number_of_sites);
            m_system.register_property(&m_origins);
        }

        void rows::site(entity::entity row, entity::entity site) {
            m_sites[m_system.lookup(row)] = site;
        }

        void rows::number_of_sites(entity::entity row, unsigned number_of_sites) {
            m_number_of_sites[m_system.lookup(row)] = number_of_sites;
        }

        void rows::origin(entity::entity row, rows::point origin) {
            m_origins[m_system.lookup(row)] = origin;
        }
    }
}
