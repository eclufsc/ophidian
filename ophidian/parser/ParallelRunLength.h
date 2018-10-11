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
        template <class T> using container_type = std::vector<T>;

        using micrometer_type = util::micrometer_t;
        using micrometer_container_type         = container_type<micrometer_type>;

        using parallelRunLength_container_type  = std::map<std::pair<micrometer_type, micrometer_type>, micrometer_type>;

        ParallelRunLength() = delete;

        ParallelRunLength(const ParallelRunLength&) = delete;
        ParallelRunLength& operator=(const ParallelRunLength&) = delete;

        ParallelRunLength(ParallelRunLength&&) = default;
        ParallelRunLength& operator=(ParallelRunLength&&) = default;

        template<class A1, class A2>
        ParallelRunLength(A1&& length, A2&& width):
            m_numLength{std::forward<A1>(length)},
            m_numWidth{std::forward<A2>(width)}
        {
            m_lengths.reserve(m_numLength);
            m_widths.reserve(m_numWidth);
        }

        void add_length(micrometer_type length) noexcept;
        void add_width(micrometer_type width) noexcept;
        void add_spacing(micrometer_type width, micrometer_type length, micrometer_type spacing) noexcept;

        int numLength() const noexcept;

        int numWidth() const noexcept;

        const micrometer_container_type& lengths() const noexcept;

        const micrometer_container_type& widths() const noexcept;

        const parallelRunLength_container_type& values() const noexcept;

        const micrometer_type& spacing(const micrometer_type width, const micrometer_type length) const noexcept;

    private:
        int m_numLength;
        int m_numWidth;
        micrometer_container_type m_lengths;
        micrometer_container_type m_widths;
        parallelRunLength_container_type m_values; //map<pair<width, length>, spacing>
    };
}

#endif
