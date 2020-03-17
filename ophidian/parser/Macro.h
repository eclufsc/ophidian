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

#ifndef OPHIDIAN_PARSER_MACRO_H
#define OPHIDIAN_PARSER_MACRO_H

#include <ophidian/geometry/Models.h>
#include <ophidian/util/Units.h>

namespace ophidian::parser
{
    class Macro
    {
    public:
        // Class member types
        struct Macro_foreign;
        class Pin;

        template <class T> using container_type    = std::vector<T>;
        template <class K, class V> using map_type = std::map<K,V>;

        template <class T> using point_type     = geometry::Point<T>;
        template <class T> using box_type       = geometry::Box<T>;

        using micrometer_type                   = util::micrometer_t;
        using micrometer_point_type             = point_type<micrometer_type>;
        using micrometer_box_type               = box_type<micrometer_type>;

        using string_type                       = std::string;

        using foreign_type                      = Macro_foreign;

        using pin_type                          = Pin;
        using pin_container_type                = container_type<pin_type>;

        using obstruction_type                  = micrometer_box_type;
        using obstruction_container_type        = container_type<obstruction_type>;
        using obstruction_map_key_type          = string_type;
        using obstruction_map_mapped_type       = obstruction_container_type;
        using obstruction_map_type              = map_type<obstruction_map_key_type, obstruction_map_mapped_type>;

        struct Macro_foreign
        {
            string_type           name;
            micrometer_point_type offset;
        };

        // Class constructors
        Macro() = delete;

        Macro(const Macro&) = default;
        Macro& operator=(const Macro&) = default;

        Macro(Macro&&) = default;
        Macro& operator=(Macro&&) = default;

        template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
        Macro(A1&& name, A2&& class_name, A3&& foreign, A4&& origin, A5&& size, A6&& site, A7&& pins, A8&& obstructions):
            m_name{std::forward<A1>(name)},
            m_class_name{std::forward<A2>(class_name)},
            m_foreign{std::forward<A3>(foreign)},
            m_origin{std::forward<A4>(origin)},
            m_size{std::forward<A5>(size)},
            m_site{std::forward<A6>(site)},
            m_pins{std::forward<A7>(pins)},
            m_obstructions{std::forward<A8>(obstructions)}
        {}

        // Class member functions
        string_type& name() noexcept;

        const string_type& name() const noexcept;

        string_type& class_name() noexcept;

        const string_type& class_name() const noexcept;

        foreign_type& foreign() noexcept;

        const foreign_type& foreign() const noexcept;

        micrometer_point_type& origin() noexcept;

        const micrometer_point_type& origin() const noexcept;

        micrometer_point_type& size() noexcept;

        const micrometer_point_type& size() const noexcept;

        string_type& site() noexcept;

        const string_type& site() const noexcept;

        pin_container_type& pins() noexcept;

        const pin_container_type& pins() const noexcept;

        obstruction_map_type& obstructions() noexcept;

        const obstruction_map_type& obstructions() const noexcept;

    private:
        string_type           m_name; ///< Name of the macro
        string_type           m_class_name; ///< Class of the macro
        foreign_type          m_foreign; ///< Struct with the foreign propertiy
        micrometer_point_type m_origin; ///< Struct containing the origin property
        micrometer_point_type m_size; ///< Struct with the size
        string_type           m_site; ///< Site name
        pin_container_type    m_pins; ///< Vector with all the macro pins
        obstruction_map_type  m_obstructions; ///< Struct with the macro rectangle geometry
    };

    class Macro::Pin
    {
    public:
        // Class member types
        enum class Direction : int {
            INPUT,
            OUTPUT,
            INOUT,
            NA
        };

        template <class T> using container_type = Macro::container_type<T>;
        template <class K, class V> using map_type    = Macro::map_type<K,V>;

        template <class T> using point_type     = Macro::point_type<T>;
        template <class T> using box_type       = Macro::box_type<T>;

        using micrometer_type                   = Macro::micrometer_type;
        using micrometer_point_type             = Macro::micrometer_point_type;
        using micrometer_box_type               = Macro::micrometer_box_type;

        using string_type                       = std::string;

        using direction_type                    = Direction;

        using port_type                         = micrometer_box_type;
        using port_container_type               = container_type<port_type>;
        using port_map_key_type                 = string_type;
        using port_map_mapped_type              = port_container_type;
        using port_map_type                     = map_type<port_map_key_type, port_map_mapped_type>;

        // Class constructors
        Pin() = delete;

        Pin(const Pin&) = delete;
        Pin& operator=(const Pin&) = delete;

        Pin(Pin&&) = default;
        Pin& operator=(Pin&&) = default;

        template<class A1, class A2, class A3>
        Pin(A1&& name, A2&& direction, A3&& ports):
            m_name{std::forward<A1>(name)},
            m_direction{std::forward<A2>(direction)},
            m_ports{std::forward<A3>(ports)}
        {}

        template<class A1, class A2, class A3, class A4>
        Pin(A1&& name, A2&& direction, A3&& ports, A4&& layer):
            m_name{std::forward<A1>(name)},
            m_direction{std::forward<A2>(direction)},
            m_ports{std::forward<A3>(ports)},
            m_layer{std::forward<A4>(layer)}
        {}

        // Class member functions
        const string_type& name() const noexcept;

        const direction_type& direction() const noexcept;

        const port_map_type& ports() const noexcept;

        const string_type & layer() const noexcept;

    private:
        string_type    m_name;
        direction_type m_direction;
        port_map_type  m_ports;
        string_type    m_layer;
    };
}

#endif
