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

#ifndef OPHIDIAN_PARSER_LAYER_H
#define OPHIDIAN_PARSER_LAYER_H

#include <ophidian/util/Units.h>

#include "EndOfLine.h"
#include "ParallelRunLength.h"
#include "AdjacentCutSpacing.h"
#include "CornerSpacing.h"

namespace ophidian::parser
{
    class Layer
    {
    public:
        // Class member types
        enum class Type : int {
            MASTERSLICE,
            CUT,
            ROUTING,
            NOT_ASSIGNED
        };

        enum class Direction : int {
            HORIZONTAL,
            VERTICAL,
            NOT_ASSIGNED
        };

        using string_type               = std::string;
        using micrometer_type           = util::micrometer_t;
        using type_type                 = Type;
        using direction_type            = Direction;
        using index_type                = int;
        using capacity_type             = int;

        using end_of_line_type            = EndOfLine;
        using parallel_run_length_type    = ParallelRunLength;
        using adjacent_cut_spacing_type   = AdjacentCutSpacing;
        using parallel_edge_type          = ParallelEdge;
        using corner_spacing_type         = CornerSpacing;

        // Class constructors
        Layer() = delete;

        Layer(const Layer&) = default;
        Layer& operator=(const Layer&) = default;

        Layer(Layer&&) = default;
        Layer& operator=(Layer&&) = default;

        template<class Arg1, class Arg2, class Arg3, class Arg4,
        class Arg5, class Arg6, class Arg7, class Arg8,
        class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14>
        Layer(Arg1&& name, Arg2&& type, Arg3&& direction,
            Arg4&& pitch, Arg5&& offset, Arg6&& width,
            Arg7&& min_width, Arg8&& area, Arg9&& spacing,
            Arg10&& eol, Arg11&& parallel_edge,
            Arg12&& parallel_run_length, Arg13&& adjacent_cut_spacing,
            Arg14&& corner_spacing
            ):

            m_name{std::forward<Arg1>(name)},
            m_type{std::forward<Arg2>(type)},
            m_direction{std::forward<Arg3>(direction)},
            m_pitch{std::forward<Arg4>(pitch)},
            m_offset{std::forward<Arg5>(offset)},
            m_width{std::forward<Arg6>(width)},
            m_min_width{std::forward<Arg7>(min_width)},
            m_area{std::forward<Arg8>(area)},
            m_spacing{std::forward<Arg9>(spacing)},
            m_end_of_line{std::forward<Arg10>(eol)},
            m_parallel_edge{std::forward<Arg11>(parallel_edge)},
            m_parallel_run_length{std::forward<Arg12>(parallel_run_length)},
            m_adjacent_cut_spacing{std::forward<Arg13>(adjacent_cut_spacing)},
            m_corner_spacing{std::forward<Arg14>(corner_spacing)}
        {}

        template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
        Layer(Arg1&& name, Arg2&& type, Arg3&& direction, Arg4&& index, Arg5&& capacity):
            m_name{std::forward<Arg1>(name)},
            m_type{std::forward<Arg2>(type)},
            m_direction{std::forward<Arg3>(direction)},
            m_index{std::forward<Arg4>(index)},
            m_capacity{std::forward<Arg5>(capacity)}
        {}

        // Class member functions
        const string_type& name() const noexcept;

        const type_type& type() const noexcept;

        const direction_type& direction() const noexcept;

        const micrometer_type& pitch() const noexcept;

        const micrometer_type& offset() const noexcept;

        const micrometer_type& width() const noexcept;

        const micrometer_type& min_width() const noexcept;

        const micrometer_type& area() const noexcept;

        const micrometer_type& spacing() const noexcept;

        const end_of_line_type& end_of_line() const noexcept;

        const parallel_edge_type& parallel_edge() const noexcept;

        const parallel_run_length_type& parallel_run_length() const noexcept;
        
        const adjacent_cut_spacing_type& adjacent_cut_spacing() const noexcept;

        const corner_spacing_type& corner_spacing() const noexcept;

        const index_type index() const noexcept;

        const capacity_type capacity() const noexcept;

        bool operator ==(const Layer& rhs) const noexcept;

        friend std::ostream& operator<<(std::ostream& os, const Layer& layer);

    private:
        string_type      m_name;
        type_type        m_type;
        direction_type   m_direction;
        micrometer_type  m_pitch;
        micrometer_type  m_offset;
        micrometer_type  m_width;
        micrometer_type  m_min_width;
        micrometer_type  m_area;
        micrometer_type  m_spacing;
        end_of_line_type m_end_of_line;
        parallel_edge_type m_parallel_edge;
        parallel_run_length_type m_parallel_run_length;
        adjacent_cut_spacing_type m_adjacent_cut_spacing;
        corner_spacing_type m_corner_spacing;
        index_type m_index;
        capacity_type m_capacity;
    };
}

#endif
