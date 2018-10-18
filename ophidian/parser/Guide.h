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

// ophidian headers
#include <ophidian/geometry/Models.h>
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

        // Class constructors
        Guide() = delete;

        Guide(const Guide&) = default;

        Guide(Guide&&) = default;

        Guide& operator=(const Guide&) = default;

        Guide& operator=(Guide&&) = default;

        Guide(const std::string& guide_file);

        // Class member functions
        void read_file(const std::string& guide_file);

        net_container_type& nets() noexcept;
        const net_container_type& nets() const noexcept;

    private:
        net_container_type m_nets;
    };

    class Guide::Net
    {
    public:
        template <class T> using container_type = std::vector<T>;
        using name_type                       = std::string;

        using region_type                       = Guide::Region;
        using region_container_type             = container_type<region_type>;

        Net() = delete;

        Net(const Net&) = default;

        Net(Net&&) = default;

        Net& operator=(const Net&) = default;

        Net& operator=(Net&&) = default;

        template<class A1, class A2>
        Net(A1&& name, A2&& regions):
            m_name{std::forward<A1>(name)},
            m_regions{std::forward<A2>(regions)}
        {}

        const name_type& name() const noexcept;

        region_container_type& regions() noexcept;
        const region_container_type& regions() const noexcept;

    private:
        name_type m_name;
        region_container_type m_regions;
    };

    class Guide::Region
    {
    public:
        // Class member types
        using geometry_type                     = Guide::database_unit_box_type;
        using layer_name_type                   = std::string;

        // Class constructors
        Region() = delete;

        Region(const Region&) = default;

        Region(Region&&) = default;

        Region& operator=(const Region&) = default;

        Region& operator=(Region&&) = default;

        template<class A1, class A2>
        Region(A1&& metal_layer_name, A2&& geometry):
            m_metal{std::forward<A1>(metal_layer_name)},
            m_region{std::forward<A2>(geometry)}
        {}

        // Class member functions
        const layer_name_type& metal_layer_name() const noexcept;

        const geometry_type& geometry() const noexcept;

    private:
        layer_name_type  m_metal; ///metal layer of guide region
        geometry_type    m_region; /// rectangle of guide region
    };
}

#endif // OPHIDIAN_PARSER_GUIDE_H
