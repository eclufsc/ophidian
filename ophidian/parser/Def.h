/*
 * Copyright 2017 Ophidian
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

#ifndef OPHIDIAN_PARSER_DEF_H
#define OPHIDIAN_PARSER_DEF_H

#include <utility>
#include <string>
#include <vector>

#include "Row.h"
#include "Component.h"
#include "Net.h"
#include "Track.h"
#include "Region.h"

#include <ophidian/geometry/Models.h>
#include <ophidian/util/Units.h>

namespace ophidian::parser
{
    /**
     * This is an encapsulation of a Def object described
     * on a DEF file to present the rows, components,
     * die area and the units of a given circuit in a usable
     * way.
     */
    class Def
    {
    public:
        // Class member types
        template <class T> using container_type = std::vector<T>;
        template <class T> using point_type     = geometry::Point<T>;
        template <class T> using box_type       = geometry::Box<T>;

        using row_type                          = Row;
        using row_container_type                = container_type<row_type>;

        using component_type                    = Component;
        using component_container_type          = container_type<component_type>;

        using track_type                        = Track;
        using track_container_type              = container_type<track_type>;

        using net_type                         = Net;
        using net_container_type               = container_type<net_type>;

        using database_unit_type                = util::database_unit_t;
        using database_unit_point_type          = point_type<database_unit_type>;
        using database_unit_box_type            = box_type<database_unit_type>;

        using scalar_type                       = util::database_unit_scalar_t;
        using scalar_point_type                 = point_type<scalar_type>;
        using scalar_box_type                   = box_type<scalar_type>;

        using region_type                       = Region;
        using region_container_type             = container_type<region_type>;

        // Class constructors
        Def() = default;

        Def(const Def&) = default;
        Def& operator=(const Def&) = default;

        Def(Def&&) = default;
        Def& operator=(Def&&) = default;

        Def(const std::string& def_file);
        Def(const std::vector<std::string>& def_files);

        // Class member functions
        void read_file(const std::string& def_file);

        const database_unit_box_type& die_area() const noexcept;

        const row_container_type& rows() const noexcept;

        const component_container_type& components() const noexcept;

        const net_container_type& nets() const noexcept;

        const scalar_type& dbu_to_micrometer_ratio() const noexcept;

        const track_container_type& tracks() const noexcept;

        const region_container_type& regions() const noexcept;

    private:
        database_unit_box_type   m_die_area{
            database_unit_point_type{database_unit_type{0.0d}, database_unit_type{0.0d}},
            database_unit_point_type{database_unit_type{0.0d}, database_unit_type{0.0d}}
        };
        row_container_type       m_rows{};
        component_container_type m_components{};
        net_container_type       m_nets{};
        scalar_type              m_dbu_to_micrometer_ratio{scalar_type{0.0d}};
        track_container_type     m_tracks{};
        region_container_type    m_regions{};
    };
}

#endif
