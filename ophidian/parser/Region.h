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

#ifndef OPHIDIAN_PARSER_REGION_H
#define OPHIDIAN_PARSER_REGION_H

#include <ophidian/util/Units.h>

namespace ophidian::parser
{
    /**
     * @brief Type to represent a circuit component.
     *
     * This is the data necessary to identify a given
     * component and it's characteristics.
     */
    class Region
    {
    public:
        template <class T> using container_type = std::vector<T>;
        template <class T> using box_type     = geometry::Box<T>;

        using name_type                = std::string;

        using database_unit_type                = util::database_unit_t;
        using database_unit_box_type          = box_type<database_unit_type>;
        using rectangles_container_type = container_type<database_unit_box_type>;

        // Class constructors

        Region() = delete;

        Region(const Region&) = default;
        Region& operator=(Region&) = default;

        Region(Region&&) = default;
        Region& operator=(Region&&) = default;

        template<class Arg1, class Arg2>
        Region(Arg1&& name, Arg2&& rectangles):
            m_name{std::forward<Arg1>(name)},
            m_rectangles{std::forward<Arg2>(rectangles)}
        {}

        // Class member functions
        const name_type& name() const noexcept;

        const rectangles_container_type & rectangles() const noexcept;

        bool operator==(const Region& rhs) const noexcept;

        friend std::ostream& operator<<(std::ostream& os, const Region& region);

    private:
        name_type                m_name; 
        rectangles_container_type m_rectangles;
    };
}

#endif
