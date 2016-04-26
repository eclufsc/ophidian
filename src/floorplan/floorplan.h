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

#ifndef ophidian_FLOORPLAN_H
#define ophidian_FLOORPLAN_H

#include <entity.h>
#include "rows.h"
#include "sites.h"

namespace ophidian {
    /// Namespace describing floorplan entities and basic floorplan interface.
    namespace floorplan {
        /// Floorplan class.
        /**
         * This class provides the basic floorplan interface, such as site and rows insertion, site and rows properties and circuit dimensions.
         */
        class floorplan {
            using point = geometry::point<double>;

            entity::system m_sites_system;
            entity::system m_rows_system;

            sites m_sites;
            rows m_rows;

            point m_chip_origin;
            point m_chip_boundaries;

            std::unordered_map<std::string, entity::entity> m_name2site;

        public:
            floorplan();

            point chip_origin() const {
                return m_chip_origin;
            }
            void chip_origin(point chip_origin);
/// Chip boundaries getter.
            /**
             * Returns the boundaries of the chip.
             * \return Point describing the chip boundaries.
             */
            point chip_boundaries() const {
                return m_chip_boundaries;
            }
            void chip_boundaries(point chip_boundaries);

            // sites
            /// Inserts a new site.
            /**
             * Inserts a new site in the floorplan. A site has a name and dimensions associated to it.
             * \param name Name of the site, used to identify it.
             * \param dimensions Point describing the site dimensions.
             * \return The created site.
             */
            entity::entity site_insert(std::string name, point dimensions);
            /// Destroys a site.
            /**
             * Destroys an existing site.
             * \param site Site to be destroyed.
             */
            void site_destroy(entity::entity site);

            /// Returns the number of sites.
            /**
             * Returns the number of sites created in the sites system.
             * \return Number of sites.
             */
            std::size_t site_count() const {
                return m_sites_system.size();
            }

            /// Site name getter.
            /**
             * Returns the name of a site.
             * \param site Site entity to get the name.
             * \return Name of the site.
             */
            std::string site_name(entity::entity site) const {
                return m_sites.name(site);
            }
            /// Site dimensions getter.
            /**
             * Returns the dimensions of a site.
             * \param site Site entity to get the dimensions.
             * \return Point describing the site dimensions.
             */
            point site_dimensions(entity::entity site) const {
                return m_sites.dimensions(site);
            }
            /// Sites properties getter.
            /**
             * Returns the object describing sites properties.
             * \return A constant reference to the object describing sites properties.
             */
            const sites & sites_properties() const {
                return m_sites;
            }

            // rows
            /// Inserts a new row.
            /**
             * Inserts a new row in the floorplan. A row has a site, number of sites and origin.
             * \param site Entity of the site of this row.
             * \param number_of_sites Number of sites which the row consists of.
             * \param origin Point describing the row origin.
             * \return The created row.
             */
            entity::entity row_insert(entity::entity site, unsigned number_of_sites, point origin);
            entity::entity row_insert(std::string site, unsigned number_of_sites, point origin);

            /// Destroys a row.
            /**
             * Destroys an existing row.
             * \param row Row to be destroyed.
             */
            void row_destroy(entity::entity row);

            /// Returns the number of rows.
            /**
             * Returns the number of rows created in the rows system.
             * \return Number of rows.
             */
            std::size_t row_count() const {
                return m_rows_system.size();
            }

            /// Row site getter.
            /**
             * Returns the site of a row.
             * \param row Row entity to get the site.
             * \return Entity representing the site of the row.
             */
            entity::entity row_site(entity::entity row) const {
                return m_rows.site(row);
            }
            /// Row number of sites getter.
            /**
             * Returns the number of sites of a row.
             * \param row Row entity to get the number of sites.
             * \return Number of sites in that row.
             */
            unsigned row_number_of_sites(entity::entity row) const {
                return m_rows.number_of_sites(row);
            }
            /// Row origin getter.
            /**
             * Returns the origin of a row.
             * \param row Row entity to get the origin.
             * \return Point describing the origin of that row.
             */
            point row_origin(entity::entity row) const {
                return m_rows.origin(row);
            }
            /// Row dimensions getter.
            /**
             * Returns the dimensions of a row, calculated using the number of sites and site dimensions in that row.
             * \param row Row entity to gets the dimensions.
             * \return Point describing the dimensions of that row.
             */
            point row_dimensions(entity::entity row) const;
            /// Rows properties getter.
            /**
             * Returns the object describing rows properties.
             * \return A constant reference to the object describing rows properties.
             */
            const rows & rows_properties() const {
                return m_rows;
            }

            const entity::system & rows_system() const {
                return m_rows_system;
            };
        };
    }
}



#endif //ophidian_FLOORPLAN_H
