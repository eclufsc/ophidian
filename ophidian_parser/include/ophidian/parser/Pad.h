/*
 * Copyright 2018 Ophidian
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

#ifndef OPHIDIAN_PARSER_PAD_H
#define OPHIDIAN_PARSER_PAD_H

#include <ophidian/geometry/Models.h>
#include <ophidian/util/Units.h>

namespace ophidian::parser
{
    class Pad{
    public:
        template <class T> using container_type     = std::vector<T>;

        template <class T> using point_type         = geometry::Point<T>;
        template <class T> using box_type           = geometry::Box<T>;
        template <class K, class V> using map_type  = std::map<K,V>;

        enum class Orientation : int { N, W, S, E, FN, FW, FS, FE };

        enum class Direction : int { INPUT, OUTPUT, NA };

        using string_type                           = std::string;

        using database_unit_type                    = util::database_unit_t;
        using dbu_point_type                        = point_type<database_unit_type>;
        using dbu_box_type                          = box_type<database_unit_type>;

        using layer_container_type                  = container_type<dbu_box_type>;
        using layer_map_type                        = map_type<string_type, layer_container_type>;

        using orientation_type                      = Orientation;

        using direction_type                        = Direction;

        // Class constructors
        Pad() = delete;

        Pad(const Pad&) = default;
        Pad& operator=(const Pad&) = default;

        Pad(Pad&&) = default;
        Pad& operator=(Pad&&) = default;

        Pad(const string_type& name):
            m_name{name}
        {}

        Pad(string_type&& name):
            m_name(std::move(name))
        {}

        //class members
        const string_type& name() const noexcept;

        void addLayer(string_type layer, layer_container_type boxes) noexcept;
        void set_orientation(orientation_type ori) noexcept;
        void set_position(dbu_point_type pos) noexcept;
        void set_direction(direction_type dir) noexcept;

        const layer_map_type& layers() const noexcept;
        const orientation_type orientation() const noexcept;
        const dbu_point_type position() const noexcept;
        const direction_type direction() const noexcept;

    private:
        string_type m_name;
        layer_map_type m_layers;
        orientation_type m_orientation;
        dbu_point_type m_position;
        direction_type m_direction;
    };
}

#endif
