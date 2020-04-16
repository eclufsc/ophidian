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

#ifndef MULTIBOX_H
#define MULTIBOX_H

#include <vector>

#include <ophidian/util/Units.h>
#include "Models.h"
#include <map>

namespace ophidian::geometry
{
    class CellGeometry
    {
    public:
        // Member types
        using unit_type               = util::database_unit_t;
        using point_type              = Point<unit_type>;
        using layer_name_type         = std::string;
        using box_type                = Box<unit_type>;
        using geometry_type           = std::pair<box_type, layer_name_type>;
        using geometry_container_type = std::vector<geometry_type>;

        // Constructors
        CellGeometry() = default;

        CellGeometry(const CellGeometry &) = default;
        CellGeometry& operator= ( const CellGeometry & ) = default;

        CellGeometry(CellGeometry &&) = default;
        CellGeometry& operator= ( CellGeometry && ) = default;

        CellGeometry(const geometry_container_type& geometries);

        CellGeometry(geometry_container_type && geometries);

        // Element access
        geometry_type& front();
        const geometry_type& front() const;

        const box_type bounding_box() const;

        point_type center() const;
        unit_type width() const;
        unit_type height() const;

        // Iterators
        geometry_container_type::iterator begin();

        geometry_container_type::iterator end();

        geometry_container_type::const_iterator begin() const;

        geometry_container_type::const_iterator end() const;

        // Capacity
        void reserve(size_t size) noexcept;

        geometry_container_type::size_type size() const noexcept;

        // Modifiers
        void push_back(const geometry_type & geometry);

        void push_back(geometry_type && geometry);

        bool operator==(const CellGeometry & other) const noexcept
        {
            for(auto geometry1 : m_geometries)
            {
                auto box1 = geometry1.first;
                for(auto geometry2 : other.m_geometries)
                {
                    auto box2 = geometry2.first;
                    bool comparison = (box1.min_corner().x() == box2.min_corner().x()) &&
                                      (box1.min_corner().y() == box2.min_corner().y()) &&
                                      (box1.max_corner().x() == box2.max_corner().x()) &&
                                      (box1.max_corner().y() == box2.max_corner().y()) &&
                                      (geometry1.second == geometry2.second);
                    if(!comparison) {
                        return false;
                    }
                }
            }

            return true;
        }

        bool operator!=(const CellGeometry & other) const noexcept
        {
            return !(*this == other);
        }

    private:
        geometry_container_type m_geometries;
    };

    CellGeometry translate(const CellGeometry& geometry, Point<CellGeometry::unit_type> translation_point) noexcept;
}

#endif // MULTIBOX_H
