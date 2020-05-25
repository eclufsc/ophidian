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

#ifndef OPHIDIAN_PARSER_ADJACENTCUTSPACING_H
#define OPHIDIAN_PARSER_ADJACENTCUTSPACING_H

#include <ophidian/util/Units.h>

namespace ophidian::parser
{
    class AdjacentCutSpacing
    {
    public:
        // Member Types
        using scalar_type       = int;
        using spacing_type      = util::micrometer_t;
        using cut_length_type   = util::micrometer_t;

        // Constructors
        AdjacentCutSpacing() = default;

        AdjacentCutSpacing(const AdjacentCutSpacing&) = default;
        AdjacentCutSpacing& operator=(const AdjacentCutSpacing&) = default;

        AdjacentCutSpacing(AdjacentCutSpacing&&) = default;
        AdjacentCutSpacing& operator=(AdjacentCutSpacing&&) = default;

        template<class A1, class A2, class A3>
        AdjacentCutSpacing(A1&& adj_spacing, A2&& cuts, A3&& within_length):
            m_adj_spacing{std::forward<A1>(adj_spacing)},
            m_adjacent_cuts{std::forward<A2>(cuts)},
            m_cut_within_length{std::forward<A3>(within_length)}
        {}

        const spacing_type& adj_spacing() const noexcept;

        const scalar_type& cuts() const noexcept;

        const cut_length_type& cut_within_length() const noexcept;

    private:
        spacing_type        m_adj_spacing{};
        scalar_type         m_adjacent_cuts{};
        cut_length_type     m_cut_within_length{};
    };
}


#endif // OPHIDIAN_PARSER_ADJACENTCUTSPACING_H