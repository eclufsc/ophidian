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
#include <ophidian/routing/Library.h>
#include <ophidian/routing/GlobalRouting.h>

namespace ophidian::design
{
    class Design
    {
    public:
        //! Class member types
        using floorplan_type            = floorplan::Floorplan;
        using standard_cell_type        = circuit::StandardCells;
        using netlist_type              = circuit::Netlist;
        using placement_library_type    = placement::Library;
        using placement_type            = placement::Placement;
        using routing_library_type      = routing::Library;
        using global_routing_type       = routing::GlobalRouting;

        //! Design Constructor

        /*!
           \brief Constructs a design system with no properties
         */
        Design() = default;

        Design(const Design&) = delete;
        Design& operator=(const Design&) = delete;

        Design(Design&&) = delete;
        Design& operator=(Design&&) = delete;

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

        //! placement library getter

        /*!
           \brief Get the placement library.
           \return Placement Library.
         */
        placement_library_type& placement_library() noexcept;

        const placement_library_type& placement_library() const noexcept;

        //! Routing library getter

        /*!
           \brief Get the routing library.
           \return Routing Library.
         */
        routing_library_type& routing_library() noexcept;

        const routing_library_type& routing_library() const noexcept;


        //! Global Routing getter

        /*!
           \brief Get the global routing.
           \return Global Routing.
         */
        global_routing_type& global_routing() noexcept;

        const global_routing_type& global_routing() const noexcept;

    private:
        floorplan_type          m_floorplan{};
        standard_cell_type      m_standard_cells{};
        netlist_type            m_netlist{};
        placement_library_type  m_placement_library{m_standard_cells};
        placement_type          m_placement{m_netlist, m_placement_library};
        routing_library_type    m_routing_library{};
        global_routing_type     m_global_routing{m_netlist};
    };
}

#endif // OPHIDIAN_DESIGN_DESIGN_H
