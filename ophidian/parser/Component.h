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

#ifndef OPHIDIAN_PARSER_COMPONENT_H
#define OPHIDIAN_PARSER_COMPONENT_H

#include <ophidian/util/Units.h>

namespace ophidian::parser
{
    /**
     * @brief Type to represent a circuit component.
     *
     * This is the data necessary to identify a given
     * component and it's characteristics.
     */
    class Component
    {
    public:
        // Class member types
        enum class Orientation : int {
            N, S, W, E, FN, FS, FW, FE
        };

        template <class T> using point_type     = geometry::Point<T>;

        using name_type                = std::string;

        using macro_type               = std::string;

        using orientation_type         = Orientation;

        using database_unit_type                = util::database_unit_t;
        using database_unit_point_type          = point_type<database_unit_type>;

        // Class constructors

        Component() = delete;

        Component(const Component&) = default;
        Component& operator=(const Component&) = default;

        Component(Component&&) = default;
        Component& operator=(Component&&) = default;

        template<class Arg1, class Arg2, class Arg3, class Arg4>
        Component(Arg1&& name, Arg2&& macro, Arg3&& orientation, Arg4&& position, bool fixed):
            m_name{std::forward<Arg1>(name)},
            m_macro{std::forward<Arg2>(macro)},
            m_orientation{std::forward<Arg3>(orientation)},
            m_position{std::forward<Arg4>(position)},
            m_fixed{fixed}
        {}

        // Class member functions
        const name_type& name() const noexcept;

        const macro_type& macro() const noexcept;

        const orientation_type& orientation() const noexcept;

        const database_unit_point_type& position() const noexcept;

        bool fixed() const noexcept;

        bool operator==(const Component& rhs) const noexcept;

        friend std::ostream& operator<<(std::ostream& os, const Component& component);

    private:
        name_type                m_name; ///< Component's name for identification.
        macro_type               m_macro; ///< Component's type, like "NAND2_X1".
        orientation_type         m_orientation; ///< Component's orientation.
        database_unit_point_type m_position; ///< Component's lower left corner.
        bool                     m_fixed; ///< This determines if the component's position is fixed in space, @c true for fixed.
    };
}

#endif
