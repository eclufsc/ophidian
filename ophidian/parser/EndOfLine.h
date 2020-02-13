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

#ifndef OPHIDIAN_PARSER_ENDOFLINE_H
#define OPHIDIAN_PARSER_ENDOFLINE_H

#include <ophidian/util/Units.h>

namespace ophidian::parser
{
    class EndOfLine
    {
    public:
        using micrometer_type = util::micrometer_t;

        // Class constructors
        EndOfLine() = delete;

        EndOfLine(const EndOfLine&) = default;
        EndOfLine& operator=(const EndOfLine&) = default;

        EndOfLine(EndOfLine&&) = default;
        EndOfLine& operator=(EndOfLine&&) = default;

        template<class A1, class A2, class A3>
        EndOfLine(A1&& space, A2&& width, A3&& within):
            m_space{std::forward<A1>(space)},
            m_width{std::forward<A2>(width)},
            m_within{std::forward<A3>(within)}
        {}

        // Class member functions
        const micrometer_type & space() const noexcept;
        const micrometer_type & width() const noexcept;
        const micrometer_type & within() const noexcept;

    private:
        micrometer_type m_space;
        micrometer_type m_width;
        micrometer_type m_within;
    };

    class ParallelEdge : public EndOfLine
    {
    public:
        using micrometer_type = util::micrometer_t;
        
        // Class constructors
        ParallelEdge() = delete;

        ParallelEdge(const ParallelEdge&) = default;
        ParallelEdge& operator=(const ParallelEdge&) = default;

        ParallelEdge(ParallelEdge&&) = default;
        ParallelEdge& operator=(ParallelEdge&&) = default;

        template<class A1, class A2, class A3, class A4, class A5>
        ParallelEdge(A1&& space, A2&& width, A3&& within, A4&& par_space, A5&& par_within)
        : EndOfLine(space, width, within),
            m_par_space{std::forward<A4>(par_space)},
            m_par_within{std::forward<A5>(par_within)}
        {}

        // Class member functions
        const micrometer_type & par_space() const noexcept;
        const micrometer_type & par_within() const noexcept;


    private:
        micrometer_type m_par_space;
        micrometer_type m_par_within;
    };

}

#endif
