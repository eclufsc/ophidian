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

#ifndef OPHIDIAN_PARSER_PARALLELRUNLENGTH_H
#define OPHIDIAN_PARSER_PARALLELRUNLENGTH_H

#include <ophidian/util/Units.h>

namespace ophidian::parser
{
    class ParallelRunLength
    {
    public:
        //Member types
        using width_type = util::micrometer_t;
        using width_container_type = std::vector<width_type>;

        using length_type = util::micrometer_t;
        using length_container_type = std::vector<length_type>;

        using spacing_type = util::micrometer_t;
        using spacing_container_type  = std::map<std::pair<width_type, length_type>, spacing_type>;

        //Constructos
        ParallelRunLength() = default;

        ParallelRunLength(const ParallelRunLength&) = default;
        ParallelRunLength& operator=(const ParallelRunLength&) = default;

        ParallelRunLength(ParallelRunLength&&) = default;
        ParallelRunLength& operator=(ParallelRunLength&&) = default;

        template<class A1, class A2, class A3>
        ParallelRunLength(A1&& widths, A2&& lengths, A3&& width_length_to_spacing):
            m_widths{std::forward<A1>(widths)},
            m_lengths{std::forward<A2>(lengths)},
            m_width_legth_to_spacing{std::forward<A3>(width_length_to_spacing)}
        { }

        //Member methods
        const length_container_type& lengths() const noexcept;

        const width_container_type& widths() const noexcept;

        const spacing_container_type& width_length_to_spacing() const noexcept;

    private:
        width_container_type   m_widths{};
        length_container_type  m_lengths{};
        spacing_container_type m_width_legth_to_spacing{};
    };
}

#endif
