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

namespace ophidian::geometry
{
    class CellGeometry
    {
    public:
        // Member types
        using unit_type = util::database_unit_t;

        using point_type = Point<unit_type>;

        using box_type  = Box<unit_type>;
        using box_container_type = std::vector<box_type>;

        class BoxComparator {
        public:
            bool operator()(const box_type & box1, const box_type & box2) 
            {
                bool comparison = (units::math::abs(box1.min_corner().x() - box2.min_corner().x()) < unit_type(std::numeric_limits<double>::epsilon())) &&
                                      (units::math::abs(box1.min_corner().y() - box2.min_corner().y()) < unit_type(std::numeric_limits<double>::epsilon())) &&
                                      (units::math::abs(box1.max_corner().x() - box2.max_corner().x()) < unit_type(std::numeric_limits<double>::epsilon())) &&
                                      (units::math::abs(box1.max_corner().y() - box2.max_corner().y()) < unit_type(std::numeric_limits<double>::epsilon()));
                return comparison;
            }
        };

        // Constructors
        CellGeometry() = default;

        CellGeometry(const CellGeometry &) = default;
        CellGeometry& operator= ( const CellGeometry & ) = default;

        CellGeometry(CellGeometry &&) = default;
        CellGeometry& operator= ( CellGeometry && ) = default;

        CellGeometry(const box_container_type& boxes);

        CellGeometry(box_container_type&& boxes);

        // Element access
        box_type& front();
        const box_type& front() const;

        // Iterators
        box_container_type::iterator begin();

        box_container_type::iterator end();

        box_container_type::const_iterator begin() const;

        box_container_type::const_iterator end() const;

        // Capacity
        box_container_type::size_type size() const noexcept;

        // Modifiers
        void push_back(const box_type & box);

        void push_back(box_type && box);

        bool operator==(const CellGeometry & other) const noexcept
        {
            auto size1 = m_boxes.size();
            auto size2 = other.m_boxes.size();
            return size1 == size2 && std::is_permutation(m_boxes.begin(), m_boxes.end(), other.m_boxes.begin(), BoxComparator());

            // for(auto box1 : m_boxes)
            // {
            //     for(auto box2 : other.m_boxes)
            //     {
            //         bool comparison = (box1.min_corner().x() == box2.min_corner().x()) &&
            //                           (box1.min_corner().y() == box2.min_corner().y()) &&
            //                           (box1.max_corner().x() == box2.max_corner().x()) &&
            //                           (box1.max_corner().y() == box2.max_corner().y());
            //         if(!comparison) {
            //             return false;
            //         }
            //     }
            // }

            // return true;
        }

        bool operator!=(const CellGeometry & other) const noexcept
        {
            return !(*this == other);
        }

        friend std::ostream& operator<<(std::ostream& os, const CellGeometry& geometry)
        {
            for (auto box : geometry.m_boxes) {
                os << box.min_corner().x() << ", " << box.min_corner().y() << " -> " 
                   << box.max_corner().x() << ", " << box.max_corner().y() << std::endl;
            }
            return os;
        }

    private:
        box_container_type m_boxes;
    };

    CellGeometry translate(const CellGeometry& geometry, Point<CellGeometry::unit_type> translation_point) noexcept;
}

#endif // MULTIBOX_H
