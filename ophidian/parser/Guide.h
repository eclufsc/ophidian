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

#ifndef OPHIDIAN_PARSER_GUIDE_H
#define OPHIDIAN_PARSER_GUIDE_H

// std headers
#include <utility>
#include <string>
#include <vector>

// external headers

// ophidian headers
#include <ophidian/util/Units.h>

namespace ophidian::parser
{
    /**
     * This is an encapsulation of rectangles, called global routing guide,
     *  to represent the regions passed by the global routing result of the
     *  associated net, and the global routing guide guarantees to cover at
     *  least a fully connected detailed routing solution for the net.
     */
    class Guide
    {
    public:
        // Class member types
        class Region;
        class Net;

        template <class T> using container_type = std::vector<T>;
        template <class T> using point_type     = geometry::Point<T>;
        template <class T> using box_type       = geometry::Box<T>;

        using database_unit_type                = util::database_unit_t;
        using database_unit_point_type          = point_type<database_unit_type>;
        using database_unit_box_type            = box_type<database_unit_type>;

        using net_type                          = Net;
        using net_container_type                = container_type<net_type>;

        using string_type                       = std::string;

        // Class constructors
        Guide() = default;

        Guide(const std::string& guide_file, bool comercia_guide = false);

        // Class member functions
        void read_file(const std::string& guide_file);

        void read_comercial_file(const std::string &guide_file);

        const size_t size() const noexcept;

        const net_container_type& nets() const noexcept;

    private:
        net_container_type m_nets;
    };

    class Guide::Net
    {
    public:
        template <class T> using container_type = std::vector<T>;
        using string_type                       = std::string;

        using region_type                       = Guide::Region;
        using region_container_type             = container_type<region_type>;

        Net() = default;

        Net(string_type name):
            m_name(name)
        {}

        void add_region(region_type& region);

        const string_type& name() const noexcept;

        const region_container_type& regions() const noexcept;

        const size_t regions_size() const noexcept;

    private:
        string_type m_name;
        region_container_type m_regions;
    };

    class Guide::Region
    {
    public:
        // Class member types
        using point_type                        = Guide::database_unit_point_type;
        using box_type                          = Guide::database_unit_box_type;
        using string_type                       = std::string;

        // Class constructors
        Region() = default;

        Region(point_type origin, point_type upper_right_corner, string_type metal):
            m_metal(metal),
            m_region(origin, upper_right_corner)
        {}

        // Class member functions
        const string_type& metal() const noexcept;

        const box_type& region() const noexcept;

    private:
        string_type m_metal; ///metal layer of guide region
        box_type    m_region; /// rectangle of guide region
    };

}

#endif // OPHIDIAN_PARSER_GUIDE_H
