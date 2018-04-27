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

#ifndef OPHIDIAN_DESIGN_DESIGN_H
#define OPHIDIAN_DESIGN_DESIGN_H

#include <ophidian/circuit/Netlist.h>
#include <ophidian/floorplan/Floorplan.h>
#include <ophidian/placement/Placement.h>
#include <ophidian/placement/Library.h>
#include <ophidian/circuit/StandardCells.h>

namespace ophidian
{
namespace design
{
    class Design
    {
    public:
        //! Class member types
        using netlist_type         = circuit::Netlist;
        using floorplan_type       = floorplan::Floorplan;
        using placement_type       = placement::Placement;
        using standard_cell_type   = circuit::StandardCells;
        using library_type         = placement::Library;

        //! Design Constructor

        /*!
           \brief Constructs a design system with no properties
         */
        Design() = default;
        
        Design(const Design&) = delete;
        Design& operator=(const Design&) = delete;

        Design(Design&&) = default;
        Design& operator=(Design&&) = default;

        template<class A1, class A2, class A3, class A4, class A5, class A6>
        Design(A1&& netlist, A2&& floorplan, A3&& placement, A4&& standard_cells, A5&& library, A6&& library_mapping):
            m_netlist{std::forward<A1>(netlist)},
            m_floorplan{std::forward<A2>(floorplan)},
            m_placement{std::forward<A3>(placement)},
            m_standard_cells{std::forward<A4>(standard_cells)},
            m_library{std::forward<A5>(library)}
        {}

        //! netlist getter

        /*!
           \brief Get the netlist.
           \return Netlist.
         */
        netlist_type& netlist() noexcept;

        const netlist_type& netlist() const noexcept;

        //! floorplan getter

        /*!
           \brief Get the floorplan.
           \return Floorplan.
         */
        floorplan_type& floorplan() noexcept;

        const floorplan_type& floorplan() const noexcept;

        //! placement getter

        /*!
           \brief Get the placement.
           \return Placement.
         */
        placement_type& placement() noexcept;

        const placement_type& placement() const noexcept;

        //! standardCells getter

        /*!
           \brief Get standardCells.
           \return StandardCells.
         */
        standard_cell_type& standard_cells() noexcept;

        const standard_cell_type& standard_cells() const noexcept;

        //! library getter

        /*!
           \brief Get the library.
           \return Library.
         */
        library_type& library() noexcept;

        const library_type& library() const noexcept;
    private:
        netlist_type         m_netlist{};
        floorplan_type       m_floorplan{};
        standard_cell_type   m_standard_cells{};
        library_type         m_library{m_standard_cells};
        placement_type       m_placement{m_netlist, m_library};
    };
}     //namespace design
}     //namespace ophidian

#endif // OPHIDIAN_DESIGN_DESIGN_H
