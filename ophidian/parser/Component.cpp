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

#include "Component.h"

namespace ophidian::parser
{
    const Component::name_type& Component::name() const noexcept
    {
        return m_name;
    }

    const Component::macro_type& Component::macro() const noexcept
    {
        return m_macro;
    }

    const Component::orientation_type& Component::orientation() const noexcept
    {
        return m_orientation;
    }

    const Component::database_unit_point_type& Component::position() const noexcept
    {
        return m_position;
    }

    bool Component::fixed() const noexcept
    {
        return m_fixed;
    }

    bool Component::operator==(const Component& rhs) const noexcept
    {
        return m_name == rhs.m_name &&
            m_macro == rhs.m_macro &&
            m_orientation == rhs.m_orientation &&
            m_position.x() == rhs.m_position.x() &&
            m_position.y() == rhs.m_position.y() &&
            m_fixed == rhs.m_fixed;
    }

    std::ostream& operator<<(std::ostream& os, const Component& component)
    {
        auto orientation_string = [&]() -> std::string {
            switch(component.m_orientation){
                case Component::orientation_type::N:
                    return "N";
                case Component::orientation_type::S:
                    return "S";
                case Component::orientation_type::W:
                    return "W";
                case Component::orientation_type::E:
                    return "E";
                case Component::orientation_type::FN:
                    return "FN";
                case Component::orientation_type::FS:
                    return "FS";
                case Component::orientation_type::FW:
                    return "FW";
                case Component::orientation_type::FE:
                    return "FE";
                default:
                    return "NA";
            }
        };

        auto fixed_string = [&]() -> std::string {
            return component.m_fixed ? "true" : "false";
        };

        os << "{name: " << component.m_name
            << ", macro: " << component.m_macro
            << ", orientarion: " << orientation_string()
            << ", position: (" << component.m_position.x()
            << ", " << component.m_position.y() << ")"
            << ", fixed: " << fixed_string()
            << "}";

        return os;
    }
}
