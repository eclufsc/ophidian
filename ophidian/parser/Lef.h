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
    /**
     * This is an encapsulation of the LEF library made by
     * Cadence Design Systems to present the sites, layers
     * and macros of a circuit
     */
    class Lef
    {
    public:
        template <class T> using container_type = std::vector<T>;

        using dbu_type     = util::database_unit_t;
        using micrometer_type = units::length::micrometer_t;
        using scalar_type  = util::database_unit_scalar_t;

        using point_micrometer    = geometry::Point<micrometer_type>;
        using point_scalar = geometry::Point<scalar_type>;
        using box_micrometer      = geometry::Box<micrometer_type>;

        struct Site;
        struct Layer;
        struct Macro;

        Lef(const std::string& filename);

        Lef(Lef&&) noexcept = default;
        Lef& operator=(Lef&&) noexcept = default;

        /**
         * Returns a vector containing all the sites in the lef
         */
        const container_type<Site>& sites() const noexcept;

        /**
         * Returns a vector containing all the layers in the lef
         */
        const container_type<Layer>& layers() const noexcept;

        /**
         * Returns a vector containing all the macros in the lef
         */
        const container_type<Macro>& macros() const noexcept;

        /**
         * The return of this function is equivalent to one micron
         */
        const scalar_type& micrometer_to_dbu_ratio() const noexcept;

    private:
        container_type<Site>       m_sites;
        container_type<Layer>      m_layers;
        container_type<Macro>      m_macros;
        scalar_type               m_micrometer_to_dbu_ratio;
    };
    
    struct Lef::Site
    {
        struct Symmetry {
            bool is_x_symmetric;
            bool is_y_symmetric;
            bool is_90_symmetric;

            bool operator ==(const Symmetry& rhs) const;
        }; 

        std::string  name; ///< Name of the site
        std::string  class_name; ///< Class of the site
        micrometer_type width; ///< Width of the site
        micrometer_type height; ///< Height of the site
        Symmetry symetry;

        bool operator ==(const Site& rhs) const;
    };

    struct Lef::Layer
    {
        enum Type {
            MASTERSLICE,
            CUT,
            ROUTING
        };

        enum Direction {
            HORIZONTAL, 
            VERTICAL,
            NOT_ASSIGNED
        };

        std::string  name; ///< Name of the layer
        Type  type; ///< Type of the layer
        Direction    direction; ///< Direction of the layer
        micrometer_type pitch; ///< Pitch of the layer
        micrometer_type offset;
        micrometer_type width; ///< Width of the layer

        bool operator ==(const Layer& rhs) const;
    };

    struct Lef::Macro
    {
        struct Macro_size
        {
            micrometer_type width; ///< Width of the macro
            micrometer_type height; ///< Height of the macro
        };

        struct Macro_foreign
        {
            std::string  name; ///< Foreign cell name
            micrometer_type x_offset; ///< Offset in the x coordinate
            micrometer_type y_offset; ///< Offset in the y coordinate
        };

        struct Pin
        {
            enum Direction {
                INPUT, OUTPUT, INOUT, NA
            };

            struct Port
            {
                ///< A vector with all the rects of the port
                std::map<std::string, container_type<box_micrometer>> layer2rects; 
            };

            std::string       name; ///< The pin name
            Direction         direction; ///< The pin's direction in accordance to pin::directions
            Port ports;
        };

        struct Obstructions
        {
            /// Map with all the rectangles of obstruction
            std::map<std::string, container_type<box_micrometer>> layer2rects;
        };

        std::string      name; ///< Name of the macro
        std::string      class_name; ///< Class of the macro
        Macro_size       size; ///< Struct with the size
        Macro_foreign    foreign; ///< Struct with the foreign propertiy
        point_micrometer origin; ///< Struct containing the origin property
        std::string      site; ///< Site name
        container_type<Pin> pins; ///< Vector with all the macro pins
        Obstructions     obstructions; ///< Struct with the macro rectangle geometry
    };
}     /* namespace parser */
}     /* namespace ophidian */

#endif /* OPHIDIAN_PARSER_LEF_H */
