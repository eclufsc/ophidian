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

#ifndef OPHIDIAN_TIMING_DRIVEN_PLACEMENT_TIMING_DRIVEN_PLACEMENT_H
#define OPHIDIAN_TIMING_DRIVEN_PLACEMENT_TIMING_DRIVEN_PLACEMENT_H

#include "../timing/generic_sta.h"
#include "../timing/ceff.h"
#include "../placement/placement.h"
#include "floorplan.h"
#include "../timing/endpoints.h"
#include "flute_rc_tree_estimation.h"

#include "../timing/static_timing_analysis.h"

namespace ophidian {
namespace timingdriven_placement {

template <class IteratorType>
struct bounds {
    const IteratorType m_begin;
    const IteratorType m_end;

    const IteratorType begin() const {
        return m_begin;
    }

    const IteratorType end() const {
        return m_end;
    }
};


using Cell = entity::entity;
using Pin = entity::entity;
using StandardCellPin = entity::entity;
using StandardCell = entity::entity;
using Net = entity::entity;
using Point = geometry::point<double>;
using Geometry = geometry::multi_polygon< geometry::polygon<Point> >;
using CapacitanceType = boost::units::quantity< boost::units::si::capacitance > ;
using NetIterator = entity::bimap_iterator_adapter;
using CellIterator = entity::bimap_iterator_adapter;
using PinIterator = entity::bimap_iterator_adapter;


struct cells_geometries {
    std::vector<Cell> cells;
    std::vector<Geometry> geometries;
    cells_geometries(std::size_t size) : cells(size), geometries(size) {cells.resize(0); geometries.resize(0);}
};



class timingdriven_placement
{


    standard_cell::standard_cells m_std_cells;
    netlist::netlist m_netlist{&m_std_cells};
    floorplan::floorplan m_floorplan;
    placement::library m_placement_lib{&m_std_cells};
    placement::placement m_placement{&m_netlist, &m_placement_lib};
    entity::vector_property< interconnection::rc_tree > m_rc_trees;
    timing::design_constraints m_dc;
    flute_rc_tree_creator m_flute;
    std::set<Net> m_dirty_nets;

    // timing info is initialized in a lazy fashion >>
    const std::string & m_dot_lib_late;
    const std::string & m_dot_lib_early;
    timing::graph m_timing_graph;
    std::unique_ptr<timing::library_timing_arcs> m_tarcs;
    std::unique_ptr<timing::library> m_lib_late;
    std::unique_ptr<timing::library> m_lib_early;
    std::unique_ptr<timing::static_timing_analysis> m_sta;
    // <<

    void make_cell_nets_dirty(Cell cell);
    void update_dirty_rc_trees();
public:
    timingdriven_placement(const std::string & dot_verilog_file, const std::string & dot_def_file, const std::string & dot_lef_file, const std::string & dot_lib_late, const std::string & dot_lib_early, double clock_in_picosseconds);
    virtual ~timingdriven_placement();



    // NETLIST
    //    cell
    //! Finds a Cell by its name
    /*!
    */
    Cell cell_find(std::string name) const {
        return m_netlist.cell_find(name);
    }

    //! Gets the name of a cell
    /*!
    */
    std::string cell_name(Cell c) const {
        return m_netlist.cell_name(c);
    }

    std::size_t cell_lookup(Cell c) const {
        return m_netlist.cell_system().lookup(c);
    }

    const std::vector<Pin> & cell_pins(Cell c) const {
        return m_netlist.cell_pins(c);
    }

    //! Gets the cells
    /*!
      \return bounds<CellIterator> of Netlist's Cell Entity System
    */
    bounds<CellIterator> cells() const {
        return {m_netlist.cell_system().begin(), m_netlist.cell_system().end()};
    }

    //   net
    //! Finds a Net by its name
    /*!
      \param name the name of the net
      \return Net entity representing the Net you are looking for, an exception is thrown if there is no net called `name`
    */
    Net net_find(std::string name) const {
        return m_netlist.net_by_name(name);
    }

    //! Gets the name of a net
    /*!
    */
    std::string net_name(Net n) const {
        return m_netlist.net_name(n);
    }

    //! Gets the pin of a net
    /*!
    */
    const std::vector<Pin> & net_pins(Net net) const {
        return m_netlist.net_pins(net);
    }

    std::size_t net_lookup(Net net) const {
        return m_netlist.net_system().lookup(net);
    }

    void net_register_property(entity::property & p);

    //! Gets the nets
    /*!
      \return bounds<NetIterator> of Netlist's Net Entity System
    */
    bounds<NetIterator> nets() const {
        return {m_netlist.net_system().begin(), m_netlist.net_system().end()};
    }

    //   pin
    //! Gets the name of a pin
    /*!
    */
    std::string pin_name(Pin pin) const {
        return m_netlist.pin_name(pin);
    }

    //! Gets the net of a pin
    /*!
    */
    Net pin_net(Pin pin) const {
        return m_netlist.pin_net(pin);
    }

    std::size_t pin_lookup(Pin pin) const {
        return m_netlist.pin_system().lookup(pin);
    }

    Cell pin_owner(Pin pin) const {
        return m_netlist.pin_owner(pin);
    }

    //! Gets the pins
    /*!
      \return bounds<PinIterator> of Netlist's Pin Entity System
    */
    bounds<PinIterator> pins() const {
        return {m_netlist.pin_system().begin(), m_netlist.pin_system().end()};
    }


    // PLACEMENT
    //! Places a cell in a position
    /*!
    */
    void place_cell(Cell cell, Point destination);
    //! Gets the cell's position
    /*!
    */
    Point cell_position(Cell cell) const {
        return m_placement.cell_position(cell);
    }
    //! Gets the cell's geometry
    /*!
       \param the cell to retrieve the geometry
       \return the cell geometry, translated to the cell position
    */
    Geometry cell_geometry(Cell cell) const {
        return m_placement.cell_geometry(cell);
    }

    Point pin_position(Pin pin) const {
        return m_placement.pin_position(pin);
    }



    // TIMING
    //! Updates the timing information
    /*!
    */
    void update_timing();

    timing::TimeType late_wns() const {
        return m_sta->late_wns();
    }
    timing::TimeType early_wns() const{
        return m_sta->early_wns();
    }
    timing::TimeType late_tns() const {
        return m_sta->late_tns();
    }
    timing::TimeType early_tns() const{
        return m_sta->early_tns();
    }
    timing::TimeType early_rise_slack(Pin p) const {
        return m_sta->early_rise_slack(p);
    }
    timing::TimeType early_fall_slack(Pin p) const {
        return m_sta->early_fall_slack(p);
    }
    timing::TimeType late_rise_slack(Pin p) const {
        return m_sta->late_rise_slack(p);
    }
    timing::TimeType late_fall_slack(Pin p) const {
        return m_sta->late_fall_slack(p);
    }
    timing::TimeType early_rise_arrival(Pin p) const {
        return m_sta->early_rise_arrival(p);
    }
    timing::TimeType early_fall_arrival(Pin p) const {
        return m_sta->early_fall_arrival(p);
    }
    timing::TimeType late_rise_arrival(Pin p) const {
        return m_sta->late_rise_arrival(p);
    }
    timing::TimeType late_fall_arrival(Pin p) const {
        return m_sta->late_fall_arrival(p);
    }
    timing::TimeType early_rise_slew(Pin p) const {
        return m_sta->early_rise_slew(p);
    }
    timing::TimeType early_fall_slew(Pin p) const {
        return m_sta->early_fall_slew(p);
    }
    timing::TimeType late_rise_slew(Pin p) const {
        return m_sta->late_rise_slew(p);
    }
    timing::TimeType late_fall_slew(Pin p) const {
        return m_sta->late_fall_slew(p);
    }
    const timing::endpoints & timing_endpoints() const {
        return m_sta->timing_endpoints();
    }
};

}
}

#endif // OPHIDIAN_TIMING_DRIVEN_PLACEMENT_TIMING_DRIVEN_PLACEMENT_H
