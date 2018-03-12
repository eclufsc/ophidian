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
#include <memory>

// external headers
#include <units/units.h>

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
        using dbu_t = util::database_unit_t;
        using dbu_scalar_t = util::database_unit_scalar_t;

    public:
        struct component;

        struct row;

        /**
         * Returns the @c dieArea.
         */
        const geometry::Box<dbu_t> & die() const;

        /**
         * Returns a @c std::vector<component> with
         * all components.
         */
        const std::vector<component> & components() const;

        /**
         * Returns a @c std::vector<row> with all rows.
         */
        const std::vector<row> & rows() const;

        /**
         * Returns the DEF database units.
         */
        dbu_scalar_t dbu_to_micrometer_convertion_factor() const;

    private:
        geometry::Box<dbu_t>   mDie;
        dbu_scalar_t           mUnits;
        std::vector<component> mComponents;
        std::vector<row>       mRows;

    public:
        friend class DefParser;
    };

    /**
     * @brief Type to represent a circuit component.
     *
     * This is the data necessary to identify a given
     * component and it's characteristics.
     */
    struct Def::component
    {
        std::string            name; ///< Component's name for identification.
        std::string            macro; ///< Component's type, like "NAND2_X1".
        std::string            orientation; ///< Component's orientation, like "N" for north.
        geometry::Point<dbu_t> position; ///< Component's lower left corner.
        bool                   fixed; ///< This determines if the component's position is fixed in space, @c true for fixed.
    };

    /**
     * @brief Type to represent a circuit row.
     *
     * This if the data necessary to identify a given
     * row and it's characteristics.
     */
    struct Def::row
    {
        std::string                   name; ///< Row's name for identification.
        std::string                   site; ///< This is the site to be used by the row defined by a LEF file.
        geometry::Point<dbu_t>        origin; ///< Specifies the location of the first site in the row.
        geometry::Point<dbu_t>        step; ///< Specifies the spacing between sites in horizontal and vertical rows.
        geometry::Point<dbu_scalar_t> num; ///< Specifies the lenght and direction of the row. (x,1) horisontal line of x sites.
    };

    /**
     * DefParser uses the DEF lib to read a def file,
     * populating a def object returning a shared_ptr
     * for it.
     */
    class DefParser
    {
    public:
        std::unique_ptr<Def> readFile(const std::string & filename) const;
    };
}
}

#endif /*OPHIDIAN_PARSER_DEF_H*/