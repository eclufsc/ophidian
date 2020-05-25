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

#ifndef OPHIDIAN_PARSER_ROW_H
#define OPHIDIAN_PARSER_ROW_H

#include <ophidian/util/Units.h>

namespace ophidian::parser
{
    /**
     * @brief Type to represent a circuit row.
     *
     * This if the data necessary to identify a given
     * row and it's characteristics.
     */
    class Row
    {
    public:
        // Class member types
        template <class T> using point_type     = geometry::Point<T>;

        using name_type              = std::string;

        using site_type              = std::string;

        using database_unit_type                = util::database_unit_t;
        using database_unit_point_type          = point_type<database_unit_type>;

        using scalar_type                       = util::database_unit_scalar_t;
        using scalar_point_type                 = point_type<scalar_type>;

        // Class constructors
        Row() = delete;

        Row(const Row&) = default;
        Row& operator=(const Row&) = default;

        Row(Row&&) = default;
        Row& operator=(Row&&) = default;

        template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
        Row(Arg1&& name, Arg2&& site, Arg3&& origin, Arg4&& step, Arg5&& num):
            m_name{std::forward<Arg1>(name)},
            m_site{std::forward<Arg2>(site)},
            m_origin{std::forward<Arg3>(origin)},
            m_step{std::forward<Arg4>(step)},
            m_num{std::forward<Arg5>(num)}
        {}

        // Class member functions
        const name_type& name() const noexcept;

        const site_type& site() const noexcept;

        const database_unit_point_type& origin() const noexcept;

        const database_unit_point_type& step() const noexcept;

        const scalar_point_type& num() const noexcept;

        bool operator==(const Row& rhs) const noexcept;

        friend std::ostream& operator<<(std::ostream& os, const Row& row);

    private:
        name_type              m_name; ///< Row's name for identification.
        site_type              m_site; ///< This is the site to be used by the row defined by a LEF file.
        database_unit_point_type m_origin; ///< Specifies the location of the first site in the row.
        database_unit_point_type m_step; ///< Specifies the spacing between sites in horizontal and vertical rows.
        scalar_point_type        m_num; ///< Specifies the lenght and direction of the row. (x,1) horisontal line of x sites.
    };
}

#endif
