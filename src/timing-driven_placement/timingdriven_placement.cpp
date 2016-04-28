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
        m_flute.create_tree(m_placement, nets[i], rc_tree_late, *m_lib_late);
    }
#pragma omp barrier
    m_dirty_nets.clear();
    std::cout << "    creating rc_trees DONE!!" << std::endl << std::flush;
}

timingdriven_placement::timingdriven_placement(const std::string & dot_verilog_file, const std::string & dot_def_file, const std::string & dot_lef_file, const std::string & dot_lib_late, const std::string & dot_lib_early, double clock_in_picosseconds):
    m_dot_lib_late(dot_lib_late),
    m_dot_lib_early(dot_lib_early)
{
    floorplan::floorplan fplan;
    std::ifstream dot_v(dot_verilog_file, std::ifstream::in);
    netlist::verilog::read(dot_v, &m_netlist);
    std::ifstream dot_def(dot_def_file, std::ifstream::in);
    placement::def::read(dot_def, &m_netlist, &m_placement, &fplan);
    // TODO READ LEF
    m_dc = timing::default_design_constraints{m_netlist}.dc();
    m_dc.clock.period = clock_in_picosseconds;
    for(auto driver : m_dc.input_drivers)
        m_std_cells.pin_direction(m_netlist.pin_std_cell(m_netlist.pin_by_name(driver.port_name)), standard_cell::pin_directions::OUTPUT);
    m_std_cells.pin_direction(m_netlist.pin_std_cell(m_netlist.pin_by_name(m_dc.clock.port_name)), standard_cell::pin_directions::OUTPUT);
    m_netlist.register_net_property(&m_rc_trees);
    for(auto cell : cells())
        make_cell_nets_dirty(cell);
}

timingdriven_placement::~timingdriven_placement()
{

}

void timingdriven_placement::net_register_property(entity::property &p)
{
    m_netlist.register_net_property(&p);
}

void timingdriven_placement::place_cell(Cell cell, Point destination)
{
    if(!geometry::equals(destination, m_placement.cell_position(cell)))
    {
        m_placement.cell_position(cell, destination);
        make_cell_nets_dirty(cell);
    }
}

void timingdriven_placement::update_timing()
{
    if(!m_sta)
    { // lazy initialization of timing data
        m_tarcs.reset(new timing::library_timing_arcs{&m_std_cells});
        m_lib_late.reset(new timing::library{m_tarcs.get(), &m_std_cells});
        m_lib_early.reset(new timing::library{m_tarcs.get(), &m_std_cells});
    #pragma omp critical
        {
            timing::liberty::read(m_dot_lib_late, *m_lib_late);
        }
    #pragma omp critical
        {
            timing::liberty::read(m_dot_lib_early, *m_lib_early);
        }
        for(auto out_load : m_dc.output_loads)
        {
            auto PO_pin = m_netlist.pin_by_name(out_load.port_name);
            auto PO_std_cell_pin = m_netlist.pin_std_cell(PO_pin);
            m_std_cells.pin_direction(m_netlist.pin_std_cell(PO_pin), standard_cell::pin_directions::INPUT);
            m_lib_late->pin_capacitance(PO_std_cell_pin, CapacitanceType(out_load.pin_load*boost::units::si::femto*boost::units::si::farads));
            m_lib_early->pin_capacitance(PO_std_cell_pin, CapacitanceType(out_load.pin_load*boost::units::si::femto*boost::units::si::farads));
        }
        m_sta.reset(new timing::static_timing_analysis);
        std::cout << "building the timing graph..." << std::endl << std::flush;
        timing::graph_builder::build(m_netlist, *m_lib_late, m_dc, m_timing_graph);
        std::cout << "building the timing graph DONE" << std::endl << std::flush;
        m_sta->graph(m_timing_graph);
        m_sta->rc_trees(m_rc_trees);
        m_sta->late_lib(*m_lib_late);
        m_sta->early_lib(*m_lib_early);
        m_sta->netlist(m_netlist);
        m_sta->set_constraints(m_dc);
    }
    update_dirty_rc_trees();
    m_sta->update_timing();
}

}
}
