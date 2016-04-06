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

#include "../catch.hpp"

#include "../timing/generic_sta.h"

#include "../netlist/verilog.h"
#include "../timing/liberty.h"
#include "../timing/graph_builder.h"

#include "../timing-driven_placement/flute_rc_tree_estimation.h"
#include "../placement/lef.h"
#include "../placement/def.h"

using namespace ophidian;

TEST_CASE("generic sta", "[timing][generic_sta]")
{
    standard_cell::standard_cells std_cells;
    netlist::netlist netlist{&std_cells};

    timing::library_timing_arcs tarcs{&std_cells};
    timing::library lib(&tarcs, &std_cells);
    std::ifstream dot_v("benchmarks/simple/simple.v", std::ifstream::in);
    REQUIRE(dot_v.good());
    netlist::verilog::read(dot_v, &netlist);
    timing::liberty::read("benchmarks/simple/simple_Late.lib", lib);
    placement::library placement_lib{&std_cells};
    placement::placement placement{&netlist, &placement_lib};

    std::ifstream dot_def("benchmarks/simple/simple.def", std::ifstream::in);
    REQUIRE(dot_def.good());

    placement::def::read(dot_def, &netlist, &placement);


    std::ifstream dot_lef("benchmarks/simple/simple.lef", std::ifstream::in);
    REQUIRE(dot_lef.good());

    placement::lef::read(dot_lef, &std_cells, &placement_lib);


    entity::vector_property< interconnection::rc_tree > rc_trees;
    netlist.register_net_property(&rc_trees);

    timing::graph graph;
    timing::default_design_constraints dc(netlist);
    timing::graph_builder::build(netlist, lib, dc.dc(), graph);


    for(auto driver : dc.dc().input_drivers)
        std_cells.pin_direction(netlist.pin_std_cell(netlist.pin_by_name(driver.port_name)), standard_cell::pin_directions::OUTPUT);

    std_cells.pin_direction(netlist.pin_std_cell(netlist.pin_by_name(dc.dc().clock.port_name)), standard_cell::pin_directions::OUTPUT);
    for(auto out_load : dc.dc().output_loads)
    {
        auto PO_pin = netlist.pin_by_name(out_load.port_name);
        auto PO_std_cell_pin = netlist.pin_std_cell(PO_pin);
        std_cells.pin_direction(netlist.pin_std_cell(PO_pin), standard_cell::pin_directions::INPUT);
        lib.pin_capacitance(PO_std_cell_pin, boost::units::quantity<boost::units::si::capacitance>(out_load.pin_load*boost::units::si::femto*boost::units::si::farads));
    }


    timing::generic_sta sta(netlist, lib, graph, rc_trees);
    sta.set_constraints(dc.dc());

    timingdriven_placement::flute_rc_tree_creator flute;
    for(auto net_it : netlist.net_system())
    {
        auto net = net_it.first;
        auto & rc_tree = rc_trees[netlist.net_system().lookup(net)];
        flute.create_tree(placement, net, rc_tree, lib);
    }



    sta.run(timing::ceff{});


}
