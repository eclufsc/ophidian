/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#ifndef OPHIDIAN_TIMING_DRIVEN_PLACEMENT_TIMING_DRIVEN_PLACEMENT_H
#define OPHIDIAN_TIMING_DRIVEN_PLACEMENT_TIMING_DRIVEN_PLACEMENT_H

#include "../timing/generic_sta.h"
#include "../timing/ceff.h"
#include "../placement/placement.h"
#include "floorplan.h"
#include "../timing/endpoints.h"
#include "flute_rc_tree_estimation.h"


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
using TimeType = boost::units::quantity< boost::units::si::time > ;
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
    timing::library_timing_arcs m_tarcs{&m_std_cells};
    timing::library m_lib_late{&m_tarcs, &m_std_cells};
    timing::library m_lib_early{&m_tarcs, &m_std_cells};
    floorplan::floorplan m_floorplan;
    placement::library m_placement_lib{&m_std_cells};
    placement::placement m_placement{&m_netlist, &m_placement_lib};

    timing::graph m_timing_graph;
    entity::vector_property< interconnection::rc_tree > m_rc_trees;
    timing::design_constraints m_dc;
    flute_rc_tree_creator m_flute;


    // lazy pointers
    std::unique_ptr<timing::timing_data> m_late;
    std::unique_ptr<timing::timing_data> m_early;
    std::unique_ptr<timing::graph_and_topology> m_topology;
    std::unique_ptr<timing::generic_sta<timing::effective_capacitance_wire_model, timing::pessimistic> > m_late_sta;
    std::unique_ptr<timing::generic_sta<timing::effective_capacitance_wire_model, timing::optimistic> > m_early_sta;
    std::unique_ptr<timing::test_calculator> m_test;
    timing::endpoints m_endpoints;

    TimeType m_lwns;
    TimeType m_ewns;
    TimeType m_ltns;
    TimeType m_etns;

    std::set<Net> m_dirty_nets;


    void init_timing_data();



    void make_cell_nets_dirty(Cell cell);
    void update_dirty_rc_trees();
public:
    timingdriven_placement(const std::string & dot_verilog_file, const std::string & dot_def_file, const std::string & dot_lef_file, const std::string & dot_lib_late, const std::string & dot_lib_early, double clock_in_picosseconds);
    virtual ~timingdriven_placement();

    Point cell_position(Cell cell) const {
        return m_placement.cell_position(cell);
    }
    Geometry cell_geometry(Cell cell) const {
        return m_placement.cell_geometry(cell);
    }

    //! Finds a Cell by its name
    /*!
      \return Cell entity representing the Cell you are looking for
    */
    Cell find_cell(std::string name) const {
        return m_netlist.cell_find(name);
    }

    //! Finds a Net by its name
    /*!
      \param name the name of the net
      \return Net entity representing the Net you are looking for, an exception is thrown if there is no net called `name`
    */
    Net find_net(std::string name) const {
        return m_netlist.net_by_name(name);
    }
    std::string net_name(Net n) const {
        return m_netlist.net_name(n);
    }
    std::string pin_name(Pin pin) const {
        return m_netlist.pin_name(pin);
    }

    bounds<NetIterator> nets() const {
        return {m_netlist.net_system().begin(), m_netlist.net_system().end()};
    }

    bounds<PinIterator> pins() const {
        return {m_netlist.pin_system().begin(), m_netlist.pin_system().end()};
    }

    bounds<CellIterator> cells() const {
        return {m_netlist.cell_system().begin(), m_netlist.cell_system().end()};
    }

    std::vector<Pin> net_pins(Net net) const {
        return m_netlist.net_pins(net);
    }

    Net pin_net(Pin pin) const {
        return m_netlist.pin_net(pin);
    }






    //! Generates a `cells_geometries` object cointaining a vector of cell entities and a vector of cell geometries
    //! Both vectors have the same size
    /*!
      \return The geometries for all cells in the netlist
    */
    ophidian::timingdriven_placement::cells_geometries cells_geometries() const {
        ophidian::timingdriven_placement::cells_geometries geometries(m_netlist.cell_count());
        for(auto cell : m_netlist.cell_system())
        {
            geometries.cells.push_back(cell.first);
            geometries.geometries.push_back(m_placement.cell_geometry(cell.first));
        }
        return geometries;
    }

    void place_cell(Cell cell, Point destination);




    void update_timing();


    TimeType late_wns() const {
        return m_lwns;
    }
    TimeType early_wns() const{
        return m_ewns;
    }
    TimeType late_tns() const {
        return m_ltns;
    }
    TimeType early_tns() const{
        return m_etns;
    }

    TimeType early_rise_slack(Pin p) const {
        return m_early_sta->rise_slack(p);
    }
    TimeType early_fall_slack(Pin p) const {
        return m_early_sta->fall_slack(p);
    }
    TimeType late_rise_slack(Pin p) const {
        return m_late_sta->rise_slack(p);
    }
    TimeType late_fall_slack(Pin p) const {
        return m_late_sta->fall_slack(p);
    }

    const timing::endpoints & timing_endpoints() const;

    void propagate_rts();

    std::string cell_name(Cell c) const {
        return m_netlist.cell_name(c);
    }

private:
    void propagate_ats();
    void update_wns_and_tns();
    bool has_timing_data() const {
        return m_late_sta && m_early_sta;
    }
};

}
}

#endif // OPHIDIAN_TIMING_DRIVEN_PLACEMENT_TIMING_DRIVEN_PLACEMENT_H
