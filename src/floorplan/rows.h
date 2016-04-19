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
        /// Rows class.
        /**
         * This class describes the properties of rows initialized by the floorplan module.
         */
        class rows {
            using point = geometry::point<double>;

            entity::system & m_system;

            entity::vector_property<entity::entity> m_sites;
            entity::vector_property<unsigned> m_number_of_sites;
            entity::vector_property<point> m_origins;

        public:
            /// Constructor.
            /**
             * Rows object constructor. Register the rows properties to the rows system.
             * \param system Rows entity system.
             */
            rows(entity::system & system);

            /// Site getter.
            /**
             * Returns the site composing a row.
             * \param row Row entity to get the site.
             * \return Site entity.
             */
            entity::entity site(entity::entity row) const {
                return m_sites[m_system.lookup(row)];
            }
            /// Number of sites getter.
            /**
             * Returns the number of sites in a row.
             * \param row Row entity to get the number of sites.
             * \return Number of sites of the row.
             */
            unsigned number_of_sites(entity::entity row) const {
                return m_number_of_sites[m_system.lookup(row)];
            }
            /// Row origin getter.
            /**
             * Returns the origin of a row.
             * \param row Row entity to get the origin.
             * \return Point describing the row origin.
             */
            point origin(entity::entity row) const {
                return m_origins[m_system.lookup(row)];
            }

            /// Sites iterator getter.
            /**
             * Returns the begin and end iterator of the row sites property.
             * \return Pair with constant iterators pointing the the beginning and end of the sites property.
             */
            std::pair< std::vector<entity::entity>::const_iterator, std::vector<entity::entity>::const_iterator > sites() const {
                return std::make_pair(m_sites.begin(), m_sites.end());
            }
            /// Number of sites iterator getter.
            /**
             * Returns the begin and end iterator of the row number of sites property.
             * \return Pair with constant iterators pointing the the beginning and end of the number of sites property.
             */
            std::pair< std::vector<unsigned>::const_iterator, std::vector<unsigned>::const_iterator > number_of_sites() const {
                return std::make_pair(m_number_of_sites.begin(), m_number_of_sites.end());
            }
            /// Origins iterator getter.
            /**
             * Returns the begin and end iterator of the row origins property.
             * \return Pair with constant iterators pointing the the beginning and end of the origins property.
             */
            std::pair< std::vector<point>::const_iterator, std::vector<point>::const_iterator > origins() const {
                return std::make_pair(m_origins.begin(), m_origins.end());
            }

            /// Site setter.
            /**
             * Sets the site of a row.
             * \param row Row entity to set the site.
             * \param site Site entity to set.
             */
            void site(entity::entity row, entity::entity site);
            /// Number of sites setter.
            /**
             * Sets the number of sites of a row.
             * \param row Row entity to set the number of sites.
             * \param number_of_sites Number of sites to set.
             */
            void number_of_sites(entity::entity row, unsigned number_of_sites);
            /// Origin setter.
            /**
             * Sets the origin of a row.
             * \param row Row entity to set the origin.
             * \param origin Origin to set.
             */
            void origin(entity::entity row, point origin);
        };
    }
}


#endif //ophidian_ROWS_H
