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

namespace ophidian {
namespace geometry {

    class CellGeometry
    {
    public:
        // Member types
        using unit_type = util::database_unit_t;

        using box_type  = Box<unit_type>;
        using box_container_type = std::vector<box_type>;

        // Constructors
        CellGeometry() = default;

        CellGeometry(const CellGeometry &) = default;
        CellGeometry& operator= ( const CellGeometry & ) = default;

        CellGeometry(CellGeometry &&) = default;
        CellGeometry& operator= ( CellGeometry && ) = default;

        CellGeometry(const box_container_type& boxes):
            m_boxes{boxes}
        {}

        CellGeometry(box_container_type&& boxes):
            m_boxes{std::move(boxes)}
        {}

        // Element access

        // Iterators
        box_container_type::iterator begin()
        {
            return m_boxes.begin();
        }

        box_container_type::iterator end()
        {
            return m_boxes.end();
        }

        box_container_type::const_iterator begin() const
        {
            return m_boxes.begin();
        }

        box_container_type::const_iterator end() const
        {
            return m_boxes.end();
        }

        // Capacity
        box_container_type::size_type size() const noexcept
        {
            return m_boxes.size();
        }

        // Modifiers
        void push_back(const box_type & box)
        {
            m_boxes.push_back(box);
        }

        void push_back(box_type && box)
        {
            m_boxes.push_back(std::move(box));
        }

        bool operator==(const CellGeometry & other) const noexcept
        {
            for(auto box1 : m_boxes)
            {
                for(auto box2 : other.m_boxes)
                {
                    bool comparison = (box1.min_corner().x() == box2.min_corner().x()) &&
                                      (box1.min_corner().y() == box2.min_corner().y()) &&
                                      (box1.max_corner().x() == box2.max_corner().x()) &&
                                      (box1.max_corner().y() == box2.max_corner().y());
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
        box_container_type m_boxes;
    };

    CellGeometry translate(const CellGeometry& geometry, Point<CellGeometry::unit_type> translation_point) noexcept;
}
}

#endif // MULTIBOX_H
