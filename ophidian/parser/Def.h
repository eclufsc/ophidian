/*
 * Copyright 2017 Ophidian
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

#ifndef OPHIDIAN_PARSER_DEF_H
#define OPHIDIAN_PARSER_DEF_H

// std headers
#include <utility>
#include <string>
#include <vector>

// external headers

// ophidian headers
#include <ophidian/geometry/Models.h>
#include <ophidian/util/Units.h>

namespace ophidian::parser
{
    /**
     * This is an encapsulation of a Def object described
     * on a DEF file to present the rows, components,
     * die area and the units of a given circuit in a usable
     * way.
     */
    class Def
    {
    public:
        // Class member types
        class Row;
        class Component;
        class Track;
        class Net;

        template <class T> using container_type = std::vector<T>;
        template <class T> using point_type     = geometry::Point<T>;
        template <class T> using box_type       = geometry::Box<T>;

        using row_type                          = Row;
        using row_container_type                = container_type<row_type>;

        using component_type                    = Component;
        using component_container_type          = container_type<component_type>;

        using track_type                        = Track;
        using track_container_type              = container_type<track_type>;

        using nets_type                         = Net;
        using nets_container_type               = container_type<nets_type>;

        using database_unit_type                = util::database_unit_t;
        using database_unit_point_type          = point_type<database_unit_type>;
        using database_unit_box_type            = box_type<database_unit_type>;

        using scalar_type                       = util::database_unit_scalar_t;
        using scalar_point_type                 = point_type<scalar_type>;
        using scalar_box_type                   = box_type<scalar_type>;

        // Class constructors
        Def() = default;

        Def(const Def&) = delete;
        Def& operator=(const Def&) = delete;

        Def(Def&&) = default;
        Def& operator=(Def&&) = default;

        Def(const std::string& def_file);
        Def(const std::vector<std::string>& def_files);

        // Class member functions
        void read_file(const std::string& def_file);

        const database_unit_box_type& die_area() const noexcept;

        const row_container_type& rows() const noexcept;

        const component_container_type& components() const noexcept;

        const scalar_type& dbu_to_micrometer_ratio() const noexcept;

        const track_container_type& tracks() const noexcept;

        const nets_container_type& nets() const noexcept;

    private:
        database_unit_box_type   m_die_area;
        row_container_type       m_rows;
        component_container_type m_components;
        scalar_type              m_dbu_to_micrometer_ratio;
        track_container_type     m_tracks;
        nets_container_type      m_nets;
    };

    class Def::Net
    {
    public:
        using string_type               = std::string;
        using scalar_type               = Def::scalar_type;
        using pin_pair_type             = std::pair<string_type, string_type>; //pair<cell, pin>
        using pin_container_type        = Def::container_type<pin_pair_type>;

        // Class constructors

        Net() = delete;

        Net(const Net&) = default;
        Net& operator=(const Net&) = default;

        Net(Net&&) = default;
        Net& operator=(Net&&) = default;

        template<class Arg1, class Arg2>
        Net(Arg1&& name, Arg2&& num_pins):
            m_name{std::forward<Arg1>(name)},
            m_numPins{std::forward<Arg2>(num_pins)}
        {
            m_pins.reserve(m_numPins);
        }

        const string_type& name() const noexcept;

        void add_connection(const string_type cell, const string_type pin);

    private:
        string_type m_name;
        scalar_type m_numPins;
        pin_container_type m_pins;
    };

    /**
     * @brief Type to represent a circuit track.
     *
     * Defines the routing grid for a standard cell-based design.
     * Typically, the routing grid is generated when the floorplan is initialized.
     * The first track is located at an offset from the placement grid set by the
     * OFFSET value for the layer in the LEF file. The track spacing is the PITCH
     * value for the layer defined in LEF.
     */
    class Def::Track
    {
    public:
        // Class member types
        enum class Orientation : int {
            X, Y
        };
        using string_type               = std::string;
        using database_unit_type        = Def::database_unit_type;
        using scalar_type               = Def::scalar_type;
        using orientation_type         = Orientation;

        // Class constructors

        Track() = delete;

        Track(const Track&) = default;
        Track& operator=(const Track&) = default;

        Track(Track&&) = default;
        Track& operator=(Track&&) = default;

        template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
        Track(Arg1&& orientation, Arg2&& start, Arg3&& numTracks, Arg4&& space, Arg5&& layer):
            m_orientation{std::forward<Arg1>(orientation)},
            m_start{std::forward<Arg2>(start)},
            m_numtracks{std::forward<Arg3>(numTracks)},
            m_space{std::forward<Arg4>(space)},
            m_layerName{std::forward<Arg5>(layer)}
        {}

        // Class member functions
        const Orientation& orientation() const noexcept;

        const database_unit_type& start() const noexcept;

        const scalar_type& numtracks() const noexcept;

        const database_unit_type& space() const noexcept;

        const string_type& layerName() const noexcept;

    private:
        Orientation m_orientation;///Specifies the location and direction of the first track defined. X indicates vertical lines; Y indicates horizontal lines.
        database_unit_type m_start;/// is the X or Y coordinate of the first line.
        scalar_type m_numtracks;///Specifies the number of tracks to create for the grid.
        database_unit_type m_space;///Specifies the spacing between the tracks.
        string_type m_layerName;///Specifies the routing layer used for the tracks.
    };

    /**
     * @brief Type to represent a circuit component.
     *
     * This is the data necessary to identify a given
     * component and it's characteristics.
     */
    class Def::Component
    {
    public:
        // Class member types
        enum class Orientation : int {
            N, S, W, E, FN, FS, FW, FE
        };

        using string_type              = std::string;
        using database_unit_type       = Def::database_unit_type;
        using database_unit_point_type = Def::database_unit_point_type;
        using orientation_type         = Orientation;

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
        const string_type& name() const noexcept;

        const string_type& macro() const noexcept;

        const orientation_type& orientation() const noexcept;

        const database_unit_point_type& position() const noexcept;

        bool fixed() const noexcept;

        bool operator==(const Component& rhs) const noexcept;

        friend std::ostream& operator<<(std::ostream& os, const Component& component);

    private:
        string_type              m_name; ///< Component's name for identification.
        string_type              m_macro; ///< Component's type, like "NAND2_X1".
        orientation_type         m_orientation; ///< Component's orientation.
        database_unit_point_type m_position; ///< Component's lower left corner.
        bool                     m_fixed; ///< This determines if the component's position is fixed in space, @c true for fixed.
    };

    /**
     * @brief Type to represent a circuit row.
     *
     * This if the data necessary to identify a given
     * row and it's characteristics.
     */
    class Def::Row
    {
    public:
        // Class member types
        using string_type              = std::string;

        using database_unit_type       = Def::database_unit_type;
        using database_unit_point_type = Def::database_unit_point_type;

        using scalar_type              = Def::scalar_type;
        using scalar_point_type        = Def::scalar_point_type;

        // Class constructors
        Row() = delete;

        Row(const Row&) = default;
        Row& operator=(const Row&) = default;

        Row(Row&&) = default;
        Row& operator=(Row&&) = default;

        template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
        Row(Arg1&& name, Arg2&& site, Arg3&& origin, Arg4&& step, Arg5&& num):
            m_name{std::forward<Arg1>(name)},
            m_site{std::forward<Arg2>(site)},
            m_origin{std::forward<Arg3>(origin)},
            m_step{std::forward<Arg4>(step)},
            m_num{std::forward<Arg5>(num)}
        {}

        // Class member functions
        const string_type& name() const noexcept;

        const string_type& site() const noexcept;

        const database_unit_point_type& origin() const noexcept;

        const database_unit_point_type& step() const noexcept;

        const scalar_point_type& num() const noexcept;

        bool operator==(const Row& rhs) const noexcept;

        friend std::ostream& operator<<(std::ostream& os, const Row& row);

    private:
        string_type              m_name; ///< Row's name for identification.
        string_type              m_site; ///< This is the site to be used by the row defined by a LEF file.
        database_unit_point_type m_origin; ///< Specifies the location of the first site in the row.
        database_unit_point_type m_step; ///< Specifies the spacing between sites in horizontal and vertical rows.
        scalar_point_type        m_num; ///< Specifies the lenght and direction of the row. (x,1) horisontal line of x sites.
    };
}

#endif /*OPHIDIAN_PARSER_DEF_H*/
