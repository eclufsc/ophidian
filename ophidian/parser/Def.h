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
#include <string>
#include <vector>

// external headers

// ophidian headers
#include <ophidian/geometry/Models.h>
#include <ophidian/util/Units.h>

namespace ophidian
{
namespace parser
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
        template <class T> using container_type = std::vector<T>;

        using dbu_type     = util::database_unit_t;
        using scalar_type  = util::database_unit_scalar_t;
        using point_dbu    = geometry::Point<dbu_type>;
        using point_scalar = geometry::Point<scalar_type>;
        using box_dbu      = geometry::Box<dbu_type>;

        struct Row;
        struct Component;

        Def(const std::string& filename);

        Def(Def&&) noexcept = default;
        Def& operator=(Def&&) noexcept = default;

        /**
         * Returns the @c dieArea.
         */
        const box_dbu & die_area() const noexcept;

        /**
         * Returns a @c std::vector<row> with all rows.
         */
        const container_type<Row> & rows() const noexcept;

        /**
         * Returns a @c std::vector<component> with
         * all components.
         */
        const container_type<Component> & components() const noexcept;

        /**
         * Returns the DEF database units.
         */
        const scalar_type dbu_to_micrometer_ratio() const noexcept;

    private:
        box_dbu                   m_die_area;
        container_type<Row>       m_rows;
        container_type<Component> m_components;
        scalar_type               m_dbu_to_micrometer_ratio;
    };

    /**
     * @brief Type to represent a circuit component.
     *
     * This is the data necessary to identify a given
     * component and it's characteristics.
     */
    struct Def::Component
    {
        enum Orientation {
            N, S, W, E, FN, FS, FW, FE
        };

        std::string            name; ///< Component's name for identification.
        std::string            macro; ///< Component's type, like "NAND2_X1".
        Orientation            orientation; ///< Component's orientation.
        point_dbu position; ///< Component's lower left corner.
        bool                   fixed; ///< This determines if the component's position is fixed in space, @c true for fixed.
    };

    /**
     * @brief Type to represent a circuit row.
     *
     * This if the data necessary to identify a given
     * row and it's characteristics.
     */
    struct Def::Row
    {
        std::string                   name; ///< Row's name for identification.
        std::string                   site; ///< This is the site to be used by the row defined by a LEF file.
        point_dbu        origin; ///< Specifies the location of the first site in the row.
        point_dbu        step; ///< Specifies the spacing between sites in horizontal and vertical rows.
        point_scalar num; ///< Specifies the lenght and direction of the row. (x,1) horisontal line of x sites.
    };
}
}

#endif /*OPHIDIAN_PARSER_DEF_H*/
