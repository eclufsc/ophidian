/*
 * Copyright 2016 Ophidian
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
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
