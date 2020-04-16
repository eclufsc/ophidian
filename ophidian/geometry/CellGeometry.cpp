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

#include "CellGeometry.h"

namespace ophidian::geometry
{
    CellGeometry::CellGeometry(const CellGeometry::geometry_container_type& geometries):
        m_geometries{geometries}
    {}

    CellGeometry::CellGeometry(CellGeometry::geometry_container_type&& geometries):
        m_geometries{std::move(geometries)}
    {}

    // Element access
    CellGeometry::geometry_type& CellGeometry::front()
    {
        return m_geometries.front();
    }
    const CellGeometry::geometry_type& CellGeometry::front() const
    {
        return m_geometries.front();
    }

    const CellGeometry::box_type CellGeometry::bounding_box() const
    {
        unit_type min_x{std::numeric_limits<double>::max()};
        unit_type min_y{std::numeric_limits<double>::max()};
        unit_type max_x{std::numeric_limits<double>::min()};
        unit_type max_y{std::numeric_limits<double>::min()};
        for(auto geometry : m_geometries)
        {
            auto box = geometry.first;
            min_x = std::min(box.min_corner().x(), min_x);
            min_y = std::min(box.min_corner().y(), min_y);
            max_x = std::max(box.max_corner().x(), max_x);
            max_y = std::max(box.max_corner().y(), max_y);
        }
        return box_type{point_type{min_x, min_y}, point_type{max_x, max_y}};
    }

    CellGeometry::point_type CellGeometry::center() const
    {
        auto box = bounding_box();
        return point_type{(box.min_corner().x() + box.max_corner().x())/2,
                          (box.min_corner().y() + box.max_corner().y())/2};
    }

    CellGeometry::unit_type CellGeometry::width() const
    {
        unit_type min_x{std::numeric_limits<double>::max()};
        unit_type max_x{std::numeric_limits<double>::min()};
        for(auto geometry : m_geometries)
        {
            auto box = geometry.first;
            min_x = std::min(box.min_corner().x(), min_x);
            max_x = std::max(box.max_corner().x(), max_x);
        }
        return max_x - min_x;
    }

    CellGeometry::unit_type CellGeometry::height() const
    {
        unit_type min_y{std::numeric_limits<double>::max()};
        unit_type max_y{std::numeric_limits<double>::min()};
        for(auto geometry : m_geometries)
        {
            auto box = geometry.first;
            min_y = std::min(box.min_corner().y(), min_y);
            max_y = std::max(box.max_corner().y(), max_y);
        }
        return max_y - min_y;
    }

    // Iterators
    CellGeometry::geometry_container_type::iterator CellGeometry::begin()
    {
        return m_geometries.begin();
    }

    CellGeometry::geometry_container_type::iterator CellGeometry::end()
    {
        return m_geometries.end();
    }

    CellGeometry::geometry_container_type::const_iterator CellGeometry::begin() const
    {
        return m_geometries.begin();
    }

    CellGeometry::geometry_container_type::const_iterator CellGeometry::end() const
    {
        return m_geometries.end();
    }

    // Capacity
    void CellGeometry::reserve(size_t size) noexcept
    {
        m_geometries.reserve(size);
    }

    CellGeometry::geometry_container_type::size_type CellGeometry::size() const noexcept
    {
        return m_geometries.size();
    }

    
    // Modifiers
    void CellGeometry::push_back(const CellGeometry::geometry_type & geometry)
    {
        m_geometries.push_back(geometry);
    }

    void CellGeometry::push_back(CellGeometry::geometry_type && geometry)
    {
        m_geometries.push_back(std::move(geometry));
    }

    CellGeometry translate(const CellGeometry& geometry, Point<CellGeometry::unit_type> translation_point) noexcept
    {
        auto translated_geometries = CellGeometry::geometry_container_type{};

        translated_geometries.reserve(geometry.size());

        for(const auto & geo : geometry)
        {
            auto box = geo.first;
            namespace bg = boost::geometry;

            const auto& min_corner = box.min_corner();
            const auto& max_corner = box.max_corner();

            auto casted_box = Box<double>{
                {units::unit_cast<double>(min_corner.x()), units::unit_cast<double>(min_corner.y())},
                {units::unit_cast<double>(max_corner.x()), units::unit_cast<double>(max_corner.y())}
            };

            auto result_box = Box<double>{{0.0,0.0},{0.0,0.0}};

            auto translate = bg::strategy::transform::translate_transformer<double, 2, 2>{
                units::unit_cast<double>(translation_point.x()), 
                units::unit_cast<double>(translation_point.y())
            };

            bg::transform(casted_box, result_box, translate);

            auto recast_box = CellGeometry::box_type{
                {CellGeometry::unit_type{result_box.min_corner().x()}, CellGeometry::unit_type{result_box.min_corner().y()}},
                {CellGeometry::unit_type{result_box.max_corner().x()}, CellGeometry::unit_type{result_box.max_corner().y()}}
            };

            translated_geometries.push_back(std::make_pair(recast_box, geo.second));
        }

        return CellGeometry{std::move(translated_geometries)};
    }
}
