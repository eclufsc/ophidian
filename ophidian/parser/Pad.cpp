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

#include "Pad.h"

namespace ophidian::parser
{
    const Pad::string_type& Pad::name() const noexcept
    {
        return m_name;
    }

    void Pad::addLayer(Pad::string_type layer, layer_container_type boxes) noexcept
    {
        m_layers.insert(std::make_pair(layer, boxes));
    }

    void Pad::set_orientation(Pad::orientation_type ori) noexcept
    {
        m_orientation = ori;
    }

    void Pad::set_position(Pad::dbu_point_type pos) noexcept
    {
        m_position = pos;
    }

    void Pad::set_direction(Pad::direction_type dir) noexcept
    {
        m_direction = dir;
    }

    const Pad::layer_map_type &Pad::layers() const noexcept
    {
        return m_layers;
    }

    const Pad::orientation_type Pad::orientation() const noexcept
    {
        return m_orientation;
    }

    const Pad::dbu_point_type Pad::position() const noexcept
    {
        return m_position;
    }

    const Pad::direction_type Pad::direction() const noexcept
    {
        return m_direction;
    }

}
