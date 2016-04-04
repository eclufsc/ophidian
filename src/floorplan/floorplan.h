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

#ifndef OPENEDA_FLOORPLAN_H
#define OPENEDA_FLOORPLAN_H

#include <entity.h>
#include "rows.h"
#include "sites.h"

namespace ophidian {
    namespace floorplan {
        class floorplan {
            using point = geometry::point<double>;

            entity::system m_sites_system;
            entity::system m_rows_system;

            sites m_sites;
            rows m_rows;

            point m_chip_boundaries;

        public:
            floorplan(point chip_boundaries);

            point chip_boundaries() const {
                return m_chip_boundaries;
            }

            // sites
            entity::entity site_insert(std::string name, point dimensions);
            void site_destroy(entity::entity site);

            std::size_t site_count() const {
                return m_sites_system.size();
            }

            std::string site_name(entity::entity site) const {
                return m_sites.name(site);
            }
            point site_dimensions(entity::entity site) const {
                return m_sites.dimensions(site);
            }

            const sites & sites_properties() const {
                return m_sites;
            }

            // rows
            entity::entity row_insert(entity::entity site, unsigned number_of_sites, point origin);
            void row_destroy(entity::entity row);

            std::size_t row_count() const {
                return m_rows_system.size();
            }

            entity::entity row_site(entity::entity row) const {
                return m_rows.site(row);
            }

            unsigned row_number_of_sites(entity::entity row) const {
                return m_rows.number_of_sites(row);
            }

            point row_origin(entity::entity row) const {
                return m_rows.origin(row);
            }

            point row_dimensions(entity::entity row) const;

            const rows & rows_properties() const {
                return m_rows;
            }
        };
    }
}



#endif //OPENEDA_FLOORPLAN_H
