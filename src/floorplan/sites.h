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
        /// Sites class.
        /**
         * This class describes the properties of sites initialized by the floorplan module.
         */
        class sites {
            using point = geometry::point<double>;

            entity::system & m_system;

            entity::vector_property<std::string> m_names;
            entity::vector_property<point> m_dimensions;

        public:
            /// Constructor.
            /**
             * Sites object constructor. Register the sites properties to the sites system.
             * \param system Sites entity system.
             */
            sites(entity::system & system);

            /// Name getter.
            /**
             * Returns the name of a site.
             * \param site Site entity to get the name.
             * \return Site name.
             */
            std::string name(entity::entity site) const {
                return m_names[m_system.lookup(site)];
            }

            /// Dimensions getter.
            /**
             * Returns the dimensions of a site.
             * \param site Site entity to get the dimensions.
             * \return Point describing the site dimensions.
             */
            point dimensions(entity::entity site) const {
                return m_dimensions[m_system.lookup(site)];
            }

            /// Name iterator getter.
            /**
             * Returns the begin and end iterator of the site names property.
             * \return Pair with constant iterators pointing the the beginning and end of the names property.
             */
            std::pair< std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator > names() const {
                return std::make_pair(m_names.begin(), m_names.end());
            }

            /// Dimensions iterator getter.
            /**
             * Returns the begin and end iterator of the site dimensions property.
             * \return Pair with constant iterators pointing the the beginning and end of the dimensions property.
             */
            std::pair< std::vector<point>::const_iterator, std::vector<point>::const_iterator > dimensions() const {
                return std::make_pair(m_dimensions.begin(), m_dimensions.end());
            }

            /// Name setter.
            /**
             * Sets the name of a site.
             * \param site Site entity to set the name.
             * \param name Desired name to set.
             */
            void name(entity::entity site, std::string name);
            /// Dimensions setter.
            /**
             * Sets the dimensions of a site.
             * \param site Site entity to set the dimensions.
             * \param name Desired dimensions to set.
             */
            void dimensions(entity::entity site, point dimensions);
        };
    }
}



#endif //ophidian_SITES_H
