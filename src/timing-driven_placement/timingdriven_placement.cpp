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

#include "timingdriven_placement.h"

#include "../netlist/verilog.h"
#include "../placement/def.h"
#include "../placement/lef.h"
#include "../timing/liberty.h"
#include "../timing/design_constraints.h"
#include "../timing/graph_builder.h"

#include "wns.h"

namespace ophidian {
namespace timingdriven_placement {

void timingdriven_placement::init_timing_data()
{
    std::cout << "building the timing graph..." << std::endl << std::flush;
    timing::graph_builder::build(m_netlist, m_lib_late, m_dc, m_timing_graph);
    std::cout << "building the timing graph DONE" << std::endl << std::flush;
    m_netlist.register_net_property(&m_rc_trees);
//    m_netlist.register_net_property(&rc_trees_late);
    for(auto cell : m_netlist.cell_system())
        make_cell_nets_dirty(cell.first);

    m_late.reset(new timing::timing_data(m_lib_late, m_timing_graph));
    m_early.reset(new timing::timing_data(m_lib_early, m_timing_graph));
    m_topology.reset(new timing::graph_and_topology(m_timing_graph, m_netlist, m_lib_late));
    m_late_sta.reset(new timing::generic_sta<timing::effective_capacitance_wire_model, timing::pessimistic>(*m_late, *m_topology, m_rc_trees));
    m_early_sta.reset(new timing::generic_sta<timing::effective_capacitance_wire_model, timing::optimistic>(*m_early, *m_topology, m_rc_trees));
    m_test.reset(new timing::test_calculator{*m_topology, *m_early, *m_late, TimeType(m_dc.clock.period*boost::units::si::pico*boost::units::si::seconds)});

    m_endpoints = timing::endpoints(m_netlist);
}

void timingdriven_placement::make_cell_nets_dirty(Cell cell)
{
    auto cell_pins = m_netlist.cell_pins(cell);
    for(auto pin : cell_pins)
        m_dirty_nets.insert(m_netlist.pin_net(pin));
}

void timingdriven_placement::update_dirty_rc_trees()
{
    std::cout << "    creating rc_trees for dirty nets..." << std::endl << std::flush;

    std::vector<Net> nets; //to process in parallel
    nets.reserve(m_dirty_nets.size());
    nets.insert(nets.end(), m_dirty_nets.begin(), m_dirty_nets.end());

    std::size_t i;
#pragma omp parallel for shared(nets) private(i)
    for(i = 0; i < nets.size(); ++i)
    {
        auto & rc_tree_late = m_rc_trees[m_netlist.net_system().lookup(nets[i])];
        m_flute.create_tree(m_placement, nets[i], rc_tree_late, m_lib_late);
    }
#pragma omp barrier
    m_dirty_nets.clear();
    std::cout << "    creating rc_trees DONE!!" << std::endl << std::flush;
}

timingdriven_placement::timingdriven_placement(const std::string & dot_verilog_file, const std::string & dot_def_file, const std::string & dot_lef_file, const std::string & dot_lib_late, const std::string & dot_lib_early, double clock_in_picosseconds)
{
    std::ifstream dot_v(dot_verilog_file, std::ifstream::in);
    netlist::verilog::read(dot_v, &m_netlist);
#pragma omp critical
    {
    timing::liberty::read(dot_lib_late, m_lib_late);
    }
#pragma omp critical
    {
    timing::liberty::read(dot_lib_early, m_lib_early);
    }
    std::ifstream dot_def(dot_def_file, std::ifstream::in);
    placement::def::read(dot_def, &m_netlist, &m_placement);
    std::ifstream dot_lef(dot_lef_file, std::ifstream::in);
    placement::lef::read(dot_lef, &m_std_cells, &m_placement_lib);
    m_dc = timing::default_design_constraints{m_netlist}.dc();
    m_dc.clock.period = clock_in_picosseconds;
    for(auto driver : m_dc.input_drivers)
        m_std_cells.pin_direction(m_netlist.pin_std_cell(m_netlist.pin_by_name(driver.port_name)), standard_cell::pin_directions::OUTPUT);
    m_std_cells.pin_direction(m_netlist.pin_std_cell(m_netlist.pin_by_name(m_dc.clock.port_name)), standard_cell::pin_directions::OUTPUT);
    for(auto out_load : m_dc.output_loads)
    {
        auto PO_pin = m_netlist.pin_by_name(out_load.port_name);
        auto PO_std_cell_pin = m_netlist.pin_std_cell(PO_pin);
        m_std_cells.pin_direction(m_netlist.pin_std_cell(PO_pin), standard_cell::pin_directions::INPUT);
        m_lib_late.pin_capacitance(PO_std_cell_pin, CapacitanceType(out_load.pin_load*boost::units::si::femto*boost::units::si::farads));
        m_lib_early.pin_capacitance(PO_std_cell_pin, CapacitanceType(out_load.pin_load*boost::units::si::femto*boost::units::si::farads));

    }
}

timingdriven_placement::~timingdriven_placement()
{

}

void timingdriven_placement::place_cell(Cell cell, Point destination)
{
    if(!geometry::equals(destination, m_placement.cell_position(cell)))
    {
        m_placement.cell_position(cell, destination);
        make_cell_nets_dirty(cell);
    }

}

void timingdriven_placement::propagate_ats()
{
    m_late_sta->update_ats();
    m_early_sta->update_ats();
}

void timingdriven_placement::propagate_rts()
{
    m_late_sta->update_rts();
    m_early_sta->update_rts();
}

void timingdriven_placement::update_wns_and_tns()
{
    m_lwns = timing::wns(m_endpoints, *m_late_sta).value();
    m_ewns = timing::wns(m_endpoints, *m_early_sta).value();
}

void timingdriven_placement::update_timing()
{
    if(!has_timing_data())
        init_timing_data();

    update_dirty_rc_trees();

    propagate_ats();

    m_test->compute_tests();

    propagate_rts();

    update_wns_and_tns();
}

}
}
