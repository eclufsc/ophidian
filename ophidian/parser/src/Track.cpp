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

#include <ophidian/parser/Track.h>

namespace ophidian::parser
{
    const Track::orientation_type& Track::orientation() const noexcept
    {
        return m_orientation;
    }

    const Track::database_unit_type& Track::start() const noexcept
    {
        return m_start;
    }

    const Track::database_unit_type& Track::space() const noexcept
    {
        return m_space;
    }

    const Track::layer_name_type& Track::layer_name() const noexcept
    {
        return m_layer_name;
    }

    const Track::scalar_type& Track::number_of_tracks() const noexcept
    {
        return m_number_of_tracks;
    }

    bool Track::operator==(const Track& rhs) const noexcept
    {
        return m_orientation == rhs.m_orientation &&
            m_start == rhs.m_start &&
            m_number_of_tracks == rhs.m_number_of_tracks &&
            m_space == rhs.m_space &&
            m_layer_name == rhs.m_layer_name;
    }

    std::ostream& operator<<(std::ostream& os, const Track& track)
    {
        auto orientation_string = [&]() -> std::string {
            switch(track.m_orientation){
                case Track::orientation_type::HORIZONTAL:
                    return "HORIZONTAL";
                case Track::orientation_type::VERTICAL:
                    return "VERTICAL";
                default:
                    return "NA";
            }
        };

        os << "{layer_name: " << track.m_layer_name
            << ", orientarion: " << orientation_string()
            << ", start: " << track.m_start
            << ", number_of_tracks: " << track.m_number_of_tracks
            << ", space: " << track.m_space
            << "}";

        return os;
    }
}
