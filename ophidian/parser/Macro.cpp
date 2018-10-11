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

#include "Macro.h"

namespace ophidian::parser
{
    Macro::string_type& Macro::name() noexcept
    {
        return m_name;
    }

    const Macro::string_type& Macro::name() const noexcept
    {
        return m_name;
    }

    Macro::string_type& Macro::class_name() noexcept
    {
        return m_class_name;
    }

    const Macro::string_type& Macro::class_name() const noexcept
    {
        return m_class_name;
    }

    Macro::foreign_type& Macro::foreign() noexcept
    {
        return m_foreign;
    }

    const Macro::foreign_type& Macro::foreign() const noexcept
    {
        return m_foreign;
    }

    Macro::micrometer_point_type& Macro::origin() noexcept
    {
        return m_origin;
    }

    const Macro::micrometer_point_type& Macro::origin() const noexcept
    {
        return m_origin;
    }

    Macro::micrometer_point_type& Macro::size() noexcept
    {
        return m_size;
    }

    const Macro::micrometer_point_type& Macro::size() const noexcept
    {
        return m_size;
    }

    Macro::string_type& Macro::site() noexcept
    {
        return m_site;
    }

    const Macro::string_type& Macro::site() const noexcept
    {
        return m_site;
    }

    Macro::pin_container_type& Macro::pins() noexcept
    {
        return m_pins;
    }

    const Macro::pin_container_type& Macro::pins() const noexcept
    {
        return m_pins;
    }

    Macro::obstruction_map_type& Macro::obstructions() noexcept
    {
        return m_obstructions;
    }

    const Macro::obstruction_map_type& Macro::obstructions() const noexcept
    {
        return m_obstructions;
    }

    const Macro::Pin::string_type& Macro::Pin::name() const noexcept
    {
        return m_name;
    }

    const Macro::Pin::direction_type& Macro::Pin::direction() const noexcept
    {
        return m_direction;
    }

    const Macro::Pin::port_map_type& Macro::Pin::ports() const noexcept
    {
        return m_ports;
    }
}
