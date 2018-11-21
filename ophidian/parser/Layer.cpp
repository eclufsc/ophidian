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

#include "Layer.h"

namespace ophidian::parser
{
    const Layer::string_type& Layer::name() const noexcept
    {
        return m_name;
    }

    const Layer::type_type& Layer::type() const noexcept
    {
        return m_type;
    }

    const Layer::direction_type& Layer::direction() const noexcept
    {
        return m_direction;
    }

    const Layer::micrometer_type& Layer::pitch() const noexcept
    {
        return m_pitch;
    }

    const Layer::micrometer_type& Layer::offset() const noexcept
    {
        return m_offset;
    }

    const Layer::micrometer_type& Layer::width() const noexcept
    {
        return m_width;
    }

    bool Layer::operator==(const Layer& rhs) const noexcept
    {
        return m_name == rhs.m_name &&
            m_type == rhs.m_type &&
            m_direction == rhs.m_direction &&
            m_pitch == rhs.m_pitch &&
            m_offset == rhs.m_offset &&
            m_width == rhs.m_width;
    }

    const Layer::micrometer_type& Layer::min_width() const noexcept{
        return m_min_width;
    }

    const Layer::micrometer_type& Layer::area() const noexcept{
        return m_area;
    }

    const Layer::micrometer_type& Layer::spacing() const noexcept{
        return m_spacing;
    }

    const Layer::end_of_line_type& Layer::end_of_line() const noexcept{
        return m_end_of_line;
    }

    const Layer::parallel_run_length_type& Layer::parallel_run_length() const noexcept{
        return m_parallel_run_length;
    }

    std::ostream& operator<<(std::ostream& os, const Layer& layer)
    {
        auto type_string = [&](){
            switch(layer.m_type){
                case Layer::type_type::MASTERSLICE:
                    return "MASTERSLICE";
                case Layer::type_type::CUT:
                    return "CUT";
                case Layer::type_type::ROUTING:
                    return "ROUTING";
                default:
                    return "NOT_ASSIGNED";
            }
        };

        auto direction_string = [&](){
            switch(layer.m_direction){
                case Layer::direction_type::HORIZONTAL:
                    return "HORIZONTAL";
                case Layer::direction_type::VERTICAL:
                    return "VERTICAL";
                default:
                    return "NOT_ASSIGNED";
            }
        };

        os << "{name: " << layer.m_name
            << ", type: " << type_string()
            << ", direction: " << direction_string()
            << ", pitch: " << layer.m_pitch
            << ", offset:" << layer.m_offset
            << ", width: " << layer.m_width
            << "}";

        return os;
    }
}
