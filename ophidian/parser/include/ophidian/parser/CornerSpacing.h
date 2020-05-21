/*
 * Copyright 2016 Ophidian
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

#ifndef OPHIDIAN_PARSER_CORNERSPACING_H
#define OPHIDIAN_PARSER_CORNERSPACING_H

#include <ophidian/util/Units.h>

namespace ophidian::parser
{
    class CornerSpacing
    {
    public:
        //Member types
        using width_type = util::micrometer_t;
        using spacing_type = util::micrometer_t;

        using width_to_spacing_type = std::pair<width_type, spacing_type>;
        using width_to_spacing_container_type = std::vector<width_to_spacing_type>;

        //Constructos
        CornerSpacing() = default;

        CornerSpacing(const CornerSpacing&) = default;
        CornerSpacing& operator=(const CornerSpacing&) = default;

        CornerSpacing(CornerSpacing&&) = default;
        CornerSpacing& operator=(CornerSpacing&&) = default;

        template<class A1, class A2>
        CornerSpacing(A1&& eolWidth, A2&& width_to_spacing):
            m_eol_width{std::forward<A1>(eolWidth)},
            m_width_legth_to_spacing{std::forward<A2>(width_to_spacing)}
        { }

        //Member methods

        const width_type& eol_width() const noexcept;

        const width_to_spacing_container_type& width_to_spacing() const noexcept;

    private:
    
        width_type m_eol_width;
        width_to_spacing_container_type m_width_legth_to_spacing;
    };
}

#endif // OPHIDIAN_PARSER_CORNERSPACING_H

