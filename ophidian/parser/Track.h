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

#ifndef OPHIDIAN_PARSER_TRACK_H
#define OPHIDIAN_PARSER_TRACK_H

#include <ophidian/util/Units.h>

namespace ophidian::parser
{
    /**
     * @brief Type to represent a circuit track.
     *
     * Defines the routing grid for a standard cell-based design.
     * Typically, the routing grid is generated when the floorplan is initialized.
     * The first track is located at an offset from the placement grid set by the
     * OFFSET value for the layer in the LEF file. The track spacing is the PITCH
     * value for the layer defined in LEF.
     */
    class Track
    {
    public:
        // Class member types
        enum class Orientation : int {
            HORIZONTAL, VERTICAL
        };

        using layer_name_type           = std::string;
        using database_unit_type        = util::database_unit_t;
        using scalar_type               = util::database_unit_scalar_t;
        using orientation_type          = Orientation;

        // Class constructors

        Track() = delete;

        Track(const Track&) = default;
        Track& operator=(const Track&) = default;

        Track(Track&&) = default;
        Track& operator=(Track&&) = default;

        template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
        Track(Arg1&& orientation, Arg2&& start, Arg3&& number_of_tracks, Arg4&& space, Arg5&& layer):
            m_orientation{std::forward<Arg1>(orientation)},
            m_start{std::forward<Arg2>(start)},
            m_number_of_tracks{std::forward<Arg3>(number_of_tracks)},
            m_space{std::forward<Arg4>(space)},
            m_layer_name{std::forward<Arg5>(layer)}
        {}

        // Class member functions
        const orientation_type& orientation() const noexcept;

        const database_unit_type& start() const noexcept;

        const scalar_type& number_of_tracks() const noexcept;

        const database_unit_type& space() const noexcept;

        const layer_name_type& layer_name() const noexcept;

        bool operator==(const Track& rhs) const noexcept;

        friend std::ostream& operator<<(std::ostream& os, const Track& track);

    private:
        orientation_type m_orientation;///Specifies the location and direction of the first track defined. X indicates vertical lines; Y indicates horizontal lines.
        database_unit_type m_start;/// is the X or Y coordinate of the first line.
        scalar_type m_number_of_tracks;///Specifies the number of tracks to create for the grid.
        database_unit_type m_space;///Specifies the spacing between the tracks.
        layer_name_type m_layer_name;///Specifies the routing layer used for the tracks.
    };
}

#endif
