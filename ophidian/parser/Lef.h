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

#ifndef OPHIDIAN_PARSER_LEF_H
#define OPHIDIAN_PARSER_LEF_H

// std headers
#include <string>
#include <vector>
#include <map>

// ophidian headers
#include <ophidian/geometry/Models.h>
#include <ophidian/util/Units.h>

namespace ophidian
{
namespace parser
{
    class Lef
    {
    public:
        // Class member typer
        class Site;
        class Layer;
        class Macro;
        class Via;

        template <class T> using container_type = std::vector<T>;

        template <class T> using point_type     = geometry::Point<T>;
        template <class T> using box_type       = geometry::Box<T>;

        using site_type                         = Site;
        using site_container_type               = container_type<site_type>;

        using layer_type                        = Layer;
        using layer_container_type              = container_type<layer_type>;

        using macro_type                        = Macro;
        using macro_container_type              = container_type<macro_type>;

        using via_type                          = Via;
        using via_container_type                = container_type<via_type>;

        using micrometer_type                   = util::micrometer_t;
        using micrometer_point_type             = point_type<micrometer_type>;
        using micrometer_box_type               = box_type<micrometer_type>;

        using scalar_type                       = util::database_unit_scalar_t;
        using scalar_point_type                 = point_type<scalar_type>;
        using scalar_box_type                   = box_type<scalar_type>;

        // Class constructors
        Lef() = default;

        Lef(const Lef&) = delete;
        Lef& operator=(const Lef&) = delete;

        Lef(Lef&&) = default;
        Lef& operator=(Lef&&) = default;
        
        Lef(const std::string& lef_file);
        Lef(const std::vector<std::string>& lef_files);

        //Class member functions
        void read_file(const std::string& lef_file);

        const site_container_type& sites() const noexcept;

        const layer_container_type& layers() const noexcept;

        const macro_container_type& macros() const noexcept;

        const scalar_type& micrometer_to_dbu_ratio() const noexcept;

        const via_container_type& vias() const noexcept;

    private:
        site_container_type  m_sites;
        layer_container_type m_layers;
        macro_container_type m_macros;
        via_container_type   m_vias;
        scalar_type          m_micrometer_to_dbu_ratio;
    };
    
    class Lef::Site
    {
    public:
        // Class member types
        struct Symmetry; 

        using string_type     = std::string;
        using micrometer_type = Lef::micrometer_type;
        using symmetry_type   = Symmetry;

        struct Symmetry {
            bool is_x_symmetric;
            bool is_y_symmetric;
            bool is_90_symmetric;

            bool operator ==(const Symmetry& rhs) const noexcept;
        }; 

        // Class constructors
        Site() = delete;

        Site(const Site&) = default;
        Site& operator=(const Site&) = default;

        Site(Site&&) = default;
        Site& operator=(Site&&) = default;

        template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
        Site(Arg1&& name, Arg2&& class_name, Arg3&& width, Arg4&& height, Arg5&& symmetry):
            m_name{std::forward<Arg1>(name)},
            m_class_name{std::forward<Arg2>(class_name)},
            m_width{std::forward<Arg3>(width)},
            m_height{std::forward<Arg4>(height)},
            m_symmetry{std::forward<Arg5>(symmetry)}
        {}

        // Class member functions
        const string_type& name() const noexcept;

        const string_type& class_name() const noexcept;

        const micrometer_type& width() const noexcept;

        const micrometer_type& height() const noexcept;

        const symmetry_type& symmetry() const noexcept;

        bool operator ==(const Site& rhs) const noexcept;

        friend std::ostream& operator<<(std::ostream& os, const Site& site);

    private:
        string_type     m_name;
        string_type     m_class_name;
        micrometer_type m_width;
        micrometer_type m_height;
        symmetry_type   m_symmetry;
    };



    class Lef::Layer
    {
    public:
        // Class member types
        class EndOfLine{
        public:
            using micrometer_type = Lef::micrometer_type;

            // Class constructors
            EndOfLine() = delete;

            EndOfLine(const EndOfLine&) = delete;
            EndOfLine& operator=(const EndOfLine&) = delete;

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

        class ParallelRunLength{
        public:
            template <class T> using container_type = std::vector<T>;

            using micrometer_type                   = Lef::micrometer_type;
            using micrometer_container_type         = container_type<micrometer_type>;

            using parallelRunLength_container_type  = std::map<std::pair<micrometer_type, micrometer_type>, micrometer_type>;

            ParallelRunLength() = delete;

            ParallelRunLength(const ParallelRunLength&) = delete;
            ParallelRunLength& operator=(const ParallelRunLength&) = delete;

            ParallelRunLength(ParallelRunLength&&) = default;
            ParallelRunLength& operator=(ParallelRunLength&&) = default;

            template<class A1, class A2>
            ParallelRunLength(A1&& length, A2&& width):
                m_numLength{std::forward<A1>(length)},
                m_numWidth{std::forward<A2>(width)}
            {
                m_lengths.reserve(m_numLength);
                m_widths.reserve(m_numWidth);
            }

            void add_length(micrometer_type length) noexcept;
            void add_width(micrometer_type width) noexcept;
            void add_spacing(micrometer_type width, micrometer_type length, micrometer_type spacing) noexcept;

            int numLength() const noexcept;

            int numWidth() const noexcept;

            const micrometer_container_type& lengths() const noexcept;

            const micrometer_container_type& widths() const noexcept;

            const parallelRunLength_container_type& values() const noexcept;

        private:
            int m_numLength;
            int m_numWidth;
            micrometer_container_type m_lengths;
            micrometer_container_type m_widths;
            parallelRunLength_container_type m_values; //map<pair<width, length>, spacing>
        };

        enum class Type : int {
            MASTERSLICE,
            CUT,
            ROUTING,
            NOT_ASSIGNED
        };

        enum class Direction : int {
            HORIZONTAL,
            VERTICAL,
            NOT_ASSIGNED
        };

        using string_type               = std::string;
        using micrometer_type           = Lef::micrometer_type;
        using type_type                 = Type;
        using direction_type            = Direction;

        using endOfLine_type            = EndOfLine;
        using parallelRunLength_type    = ParallelRunLength;

        // Class constructors
        Layer() = delete;

        Layer(const Layer&) = default;
        Layer& operator=(const Layer&) = default;

        Layer(Layer&&) = default;
        Layer& operator=(Layer&&) = default;

        template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11>
        Layer(Arg1&& name, Arg2&& type, Arg3&& direction, Arg4&& pitch, Arg5&& offset, Arg6&& width, Arg7&& min_width, Arg8&& area, Arg9&& spacing, Arg10&& eol, Arg11&& parallelRunLength):
            m_name{std::forward<Arg1>(name)},
            m_type{std::forward<Arg2>(type)},
            m_direction{std::forward<Arg3>(direction)},
            m_pitch{std::forward<Arg4>(pitch)},
            m_offset{std::forward<Arg5>(offset)},
            m_width{std::forward<Arg6>(width)},

            m_min_width{std::forward<Arg7>(min_width)},
            m_area{std::forward<Arg8>(area)},
            m_spacing{std::forward<Arg9>(spacing)},
            m_eol{std::forward<Arg10>(eol)},
            m_parallelRunLength{std::forward<Arg11>(parallelRunLength)}
        {}

        // Class member functions
        const string_type& name() const noexcept;

        const type_type& type() const noexcept;

        const direction_type& direction() const noexcept;

        const micrometer_type& pitch() const noexcept;

        const micrometer_type& offset() const noexcept;

        const micrometer_type& width() const noexcept;

        bool operator ==(const Layer& rhs) const noexcept;

        const micrometer_type& min_width() const noexcept;
        const micrometer_type& area() const noexcept;
        const micrometer_type& spacing() const noexcept;
        const endOfLine_type& EOL() const noexcept;
        const parallelRunLength_type& parallelRunLength() const noexcept;
        friend std::ostream& operator<<(std::ostream& os, const Layer& layer);

    private:
        string_type     m_name;
        type_type       m_type;
        direction_type  m_direction;
        micrometer_type m_pitch;
        micrometer_type m_offset;
        micrometer_type m_width;
        micrometer_type m_min_width;
        micrometer_type m_area;
        micrometer_type m_spacing;
        endOfLine_type  m_eol;
        parallelRunLength_type m_parallelRunLength;
    };


    class Lef::Macro
    {
    public:
        // Class member types
        struct Macro_foreign;
        class Pin;

        template <class T> using container_type    = Lef::container_type<T>;
        template <class K, class V> using map_type = std::map<K,V>;

        template <class T> using point_type     = Lef::point_type<T>;
        template <class T> using box_type       = Lef::box_type<T>;

        using micrometer_type                   = Lef::micrometer_type;
        using micrometer_point_type             = Lef::micrometer_point_type;
        using micrometer_box_type               = Lef::micrometer_box_type;

        using string_type                       = std::string;

        using foreign_type                      = Macro_foreign;

        using pin_type                          = Pin;
        using pin_container_type                = container_type<pin_type>;

        using obstruction_type                  = micrometer_box_type;
        using obstruction_container_type        = container_type<obstruction_type>;
        using obstruction_map_key_type          = string_type;
        using obstruction_map_mapped_type       = obstruction_container_type;
        using obstruction_map_type              = map_type<obstruction_map_key_type, obstruction_map_mapped_type>;

        struct Macro_foreign
        {
            string_type           name;
            micrometer_point_type offset;
        };

        // Class constructors
        Macro() = delete;

        Macro(const Macro&) = delete;
        Macro& operator=(const Macro&) = delete;

        Macro(Macro&&) = default;
        Macro& operator=(Macro&&) = default;

        template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
        Macro(A1&& name, A2&& class_name, A3&& foreign, A4&& origin, A5&& size, A6&& site, A7&& pins, A8&& obstructions):
            m_name{std::forward<A1>(name)},
            m_class_name{std::forward<A2>(class_name)},
            m_foreign{std::forward<A3>(foreign)},
            m_origin{std::forward<A4>(origin)},
            m_size{std::forward<A5>(size)},
            m_site{std::forward<A6>(site)},
            m_pins{std::forward<A7>(pins)},
            m_obstructions{std::forward<A8>(obstructions)}
        {}

        // Class member functions
        const string_type& name() const noexcept;

        const string_type& class_name() const noexcept;

        const foreign_type& foreign() const noexcept;

        const micrometer_point_type& origin() const noexcept;

        const micrometer_point_type& size() const noexcept;

        const string_type& site() const noexcept;

        const pin_container_type& pins() const noexcept;

        const obstruction_map_type& obstructions() const noexcept;

    private:
        string_type           m_name; ///< Name of the macro
        string_type           m_class_name; ///< Class of the macro
        foreign_type          m_foreign; ///< Struct with the foreign propertiy
        micrometer_point_type m_origin; ///< Struct containing the origin property
        micrometer_point_type m_size; ///< Struct with the size
        string_type           m_site; ///< Site name
        pin_container_type    m_pins; ///< Vector with all the macro pins
        obstruction_map_type  m_obstructions; ///< Struct with the macro rectangle geometry

        friend Lef;
    };

    class Lef::Macro::Pin
    {
    public:
        // Class member types
        enum class Direction : int {
            INPUT,
            OUTPUT,
            INOUT,
            NA
        };

        template <class T> using container_type = Lef::Macro::container_type<T>;
        template <class K, class V> using map_type    = Lef::Macro::map_type<K,V>;

        template <class T> using point_type     = Lef::Macro::point_type<T>;
        template <class T> using box_type       = Lef::Macro::box_type<T>;

        using micrometer_type                   = Lef::Macro::micrometer_type;
        using micrometer_point_type             = Lef::Macro::micrometer_point_type;
        using micrometer_box_type               = Lef::Macro::micrometer_box_type;

        using string_type                       = std::string;

        using direction_type                    = Direction;

        using port_type                         = micrometer_box_type;
        using port_container_type               = container_type<port_type>;
        using port_map_key_type                 = string_type;
        using port_map_mapped_type              = port_container_type;
        using port_map_type                     = map_type<port_map_key_type, port_map_mapped_type>;

        // Class constructors
        Pin() = delete;

        Pin(const Pin&) = delete;
        Pin& operator=(const Pin&) = delete;

        Pin(Pin&&) = default;
        Pin& operator=(Pin&&) = default;

        template<class A1, class A2, class A3>
        Pin(A1&& name, A2&& direction, A3&& ports):
            m_name{std::forward<A1>(name)},
            m_direction{std::forward<A2>(direction)},
            m_ports{std::forward<A3>(ports)}
        {}

        // Class member functions
        const string_type& name() const noexcept;

        const direction_type& direction() const noexcept;

        const port_map_type& ports() const noexcept;

    private:
        string_type    m_name;
        direction_type m_direction;
        port_map_type  m_ports;
    };

    class Lef::Via{
    public:
        template <class T> using container_type     = std::vector<T>;

        template <class T> using point_type         = geometry::Point<T>;
        template <class T> using box_type           = geometry::Box<T>;
        template <class K, class V> using map_type  = Lef::Macro::map_type<K,V>;

        using string_type                           = std::string;
        using micrometer_type                       = Lef::micrometer_type;
        using micrometer_point_type                 = point_type<micrometer_type>;
        using micrometer_box_type                   = box_type<micrometer_type>;

        using layer_container_type                  = container_type<micrometer_box_type>;
        using layer_map_type                        = map_type<string_type, layer_container_type>;

        // Class constructors
        Via() = delete;

        Via(const Via&) = default;
        Via& operator=(const Via&) = default;

        Via(Via&&) = default;
        Via& operator=(Via&&) = default;

        Via(string_type name):
            m_name{name}
        {}

        //class members
        void addLayer(string_type layer, layer_container_type boxes) noexcept;

        const layer_map_type& layers() const noexcept;

    private:
        string_type m_name;
        layer_map_type m_layers;
    };
}     /* namespace parser */
}     /* namespace ophidian */

#endif /* OPHIDIAN_PARSER_LEF_H */
