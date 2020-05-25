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

#ifndef OPHIDIAN_PARSER_SITE_H
#define OPHIDIAN_PARSER_SITE_H

#include <ophidian/util/Units.h>

namespace ophidian::parser
{
    class Site
    {
    public:
        // Class member types
        struct Symmetry;

        using string_type     = std::string;
        using micrometer_type = util::micrometer_t;
        using symmetry_type   = Symmetry;

        struct Symmetry {
            bool is_x_symmetric;
            bool is_y_symmetric;
            bool is_90_symmetric;

            bool operator ==(const Symmetry& rhs) const noexcept;
        };

        // Class constructors
        Site() = delete;

        Site(const Site&) = default;
        Site& operator=(const Site&) = default;

        Site(Site&&) = default;
        Site& operator=(Site&&) = default;

        template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
        Site(Arg1&& name, Arg2&& class_name, Arg3&& width, Arg4&& height, Arg5&& symmetry):
            m_name{std::forward<Arg1>(name)},
            m_class_name{std::forward<Arg2>(class_name)},
            m_width{std::forward<Arg3>(width)},
            m_height{std::forward<Arg4>(height)},
            m_symmetry{std::forward<Arg5>(symmetry)}
        {}

        // Class member functions
        const string_type& name() const noexcept;

        const string_type& class_name() const noexcept;

        const micrometer_type& width() const noexcept;

        const micrometer_type& height() const noexcept;

        const symmetry_type& symmetry() const noexcept;

        bool operator ==(const Site& rhs) const noexcept;

        friend std::ostream& operator<<(std::ostream& os, const Site& site);

    private:
        string_type     m_name;
        string_type     m_class_name;
        micrometer_type m_width;
        micrometer_type m_height;
        symmetry_type   m_symmetry;
    };
}

#endif
