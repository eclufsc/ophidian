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
#include <memory>

// external headers
#include <lefrReader.hpp>

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
        using micrometer_t = units::length::micrometer_t;
        using dbu_t = util::database_unit_t;
        using micrometer_ratio_t = util::database_unit_scalar_t;
        using point = geometry::Point<micrometer_t>;
        using box = geometry::Box<micrometer_t>;

    public:

        /**
         * @brief An enumeration to represent all the possible symmetries in a site.
         * They are numbered 1, 2 and 4 to enable binary operations to combine
         * multiple symmetries
         */
        enum symmetry {
            X= 1, Y= 2, NINETY= 4
        };

        struct site
        {
            std::string  name; ///< Name of the site
            std::string  class_name; ///< Class of the site
            micrometer_t x; ///< Width of the site
            micrometer_t y; ///< Height of the site

            /// Stores the site's symmetry in accordance to the Lef::symmetrie enumeration
            char symmetry = 0;

            void setXsymmetry(); ///< Sets the X symmetry bit

            void setYsymmetry(); ///< Sets the Y symmetry bit

            void set90symmetry(); ///< Sets the R90 symmetry bit
        };

        struct layer
        {
            enum direction {
                NOT_ASSIGNED, HORIZONTAL, VERTICAL
            };

            std::string  name; ///< Name of the layer
            std::string  type; ///< Type of the layer
            direction    direction; ///< Direction of the layer
            micrometer_t pitch; ///< Pitch of the layer
            micrometer_t offset;
            micrometer_t width; ///< Width of the layer
        };

        struct macro
        {
            struct macro_size
            {
                micrometer_t width; ///< Width of the macro
                micrometer_t height; ///< Height of the macro
            };

            struct macro_foreign
            {
                std::string  name; ///< Foreign cell name
                micrometer_t x; ///< Offset in the x coordinate
                micrometer_t y; ///< Offset in the y coordinate
            };

            struct pin
            {
                enum direction {
                    INPUT, OUTPUT, INOUT, NA
                };

                struct port
                {
                    std::vector<std::string>                 layers; ///< A vector with all the names of all the port layers
                    std::vector<geometry::Box<micrometer_t>> rects; ///< A vector with all the rects of the port
                };

                std::string       name; ///< The pin name
                direction         direction{NA}; ///< The pin's direction in accordance to pin::directions
                std::vector<port> ports;
            };

            struct obstructions
            {
                /// Map with all the rectangles of obstruction
                std::map<std::string, std::vector<geometry::Box<micrometer_t>>> layer2rects;
            };

            std::string      name; ///< Name of the macro
            std::string      class_name; ///< Class of the macro
            macro_size       size; ///< Struct with the size
            macro_foreign    foreign; ///< Struct with the foreign propertiy
            point            origin; ///< Struct containing the origin property
            std::string      site; ///< Site name
            std::vector<pin> pins; ///< Vector with all the macro pins
            obstructions     obstructions; ///< Struct with the macro rectangle geometry
        };

        /**
         * Returns a vector containing all the sites in the lef
         */
        const std::vector<site> & sites() const;

        /**
         * Returns a vector containing all the layers in the lef
         */
        const std::vector<layer> & layers() const;

        /**
         * Returns a vector containing all the macros in the lef
         */
        const std::vector<macro> & macros() const;

        /**
         * The return of this function is equivalent to one micron
         */
        micrometer_ratio_t micron_to_dbu_convertion_factor() const;

    private:
        std::vector<site>       mSites;
        std::vector<layer>      mLayers;
        std::vector<macro>      mMacros;
        LefDefParser::lefiUnits mUnits;

    public:
        friend class LefParser;
    };

    class LefParser
    {
    public:
        void readFile(const std::string & filename, std::unique_ptr<Lef> & inp) const;
    };
}     /* namespace parser */
}     /* namespace ophidian */

#endif /* OPHIDIAN_PARSER_LEF_H */
