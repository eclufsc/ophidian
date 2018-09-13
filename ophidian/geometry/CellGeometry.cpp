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
    CellGeometry::CellGeometry(const CellGeometry::box_container_type& boxes):
        m_boxes{boxes}
    {}

    CellGeometry::CellGeometry(CellGeometry::box_container_type&& boxes):
        m_boxes{std::move(boxes)}
    {}

    // Iterators
    CellGeometry::box_container_type::iterator CellGeometry::begin()
    {
        return m_boxes.begin();
    }

    CellGeometry::box_container_type::iterator CellGeometry::end()
    {
        return m_boxes.end();
    }

    CellGeometry::box_container_type::const_iterator CellGeometry::begin() const
    {
        return m_boxes.begin();
    }

    CellGeometry::box_container_type::const_iterator CellGeometry::end() const
    {
        return m_boxes.end();
    }

    // Capacity
    CellGeometry::box_container_type::size_type CellGeometry::size() const noexcept
    {
        return m_boxes.size();
    }
    
    // Modifiers
    void CellGeometry::push_back(const CellGeometry::box_type & box)
    {
        m_boxes.push_back(box);
    }

    void CellGeometry::push_back(CellGeometry::box_type && box)
    {
        m_boxes.push_back(std::move(box));
    }

    CellGeometry translate(const CellGeometry& geometry, Point<CellGeometry::unit_type> translation_point) noexcept
    {
        auto translated_boxes = CellGeometry::box_container_type{};

        translated_boxes.reserve(geometry.size());

        for(const auto & box : geometry)
        {
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

            translated_boxes.push_back(std::move(recast_box));
        }

        return CellGeometry{std::move(translated_boxes)};
    }
}
