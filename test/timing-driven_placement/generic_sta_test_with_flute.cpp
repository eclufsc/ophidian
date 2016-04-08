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

#include <boost/units/systems/si.hpp>
#include <boost/units/limits.hpp>
#include <boost/units/io.hpp>
#include <boost/units/systems/si/prefixes.hpp>

using namespace ophidian;

class simple_moc {
public:
    standard_cell::standard_cells std_cells;
    netlist::netlist netlist{&std_cells};
    timing::library_timing_arcs tarcs{&std_cells};
    timing::library lib_late{&tarcs, &std_cells};
    timing::library lib_early{&tarcs, &std_cells};
    placement::library placement_lib{&std_cells};
    placement::placement placement{&netlist, &placement_lib};
    timing::graph graph;
    entity::vector_property< interconnection::rc_tree > rc_trees_early;
    entity::vector_property< interconnection::rc_tree > rc_trees_late;
    timing::default_design_constraints dc;
    simple_moc()
    {
        std::ifstream dot_v("benchmarks/simple/simple.v", std::ifstream::in);
        netlist::verilog::read(dot_v, &netlist);
        timing::liberty::read("benchmarks/simple/simple_Late.lib", lib_late);
        timing::liberty::read("benchmarks/simple/simple_Early.lib", lib_early);
        std::ifstream dot_def("benchmarks/simple/simple.def", std::ifstream::in);
        placement::def::read(dot_def, &netlist, &placement);
        std::ifstream dot_lef("benchmarks/simple/simple.lef", std::ifstream::in);
        placement::lef::read(dot_lef, &std_cells, &placement_lib);
        dc = timing::default_design_constraints{netlist};
        for(auto driver : dc.dc().input_drivers)
            std_cells.pin_direction(netlist.pin_std_cell(netlist.pin_by_name(driver.port_name)), standard_cell::pin_directions::OUTPUT);
        std_cells.pin_direction(netlist.pin_std_cell(netlist.pin_by_name(dc.dc().clock.port_name)), standard_cell::pin_directions::OUTPUT);
        for(auto out_load : dc.dc().output_loads)
        {
            auto PO_pin = netlist.pin_by_name(out_load.port_name);
            auto PO_std_cell_pin = netlist.pin_std_cell(PO_pin);
            std_cells.pin_direction(netlist.pin_std_cell(PO_pin), standard_cell::pin_directions::INPUT);
            lib_late.pin_capacitance(PO_std_cell_pin, boost::units::quantity<boost::units::si::capacitance>(out_load.pin_load*boost::units::si::femto*boost::units::si::farads));
        }
        timing::graph_builder::build(netlist, lib_late, dc.dc(), graph);

        netlist.register_net_property(&rc_trees_early);
        netlist.register_net_property(&rc_trees_late);
        std::vector<entity::entity> nets(netlist.net_system().size());
        nets.resize(0);
        for(auto net : netlist.net_system())
            nets.push_back(net.first);

        std::size_t i;
        timingdriven_placement::flute_rc_tree_creator flute;
#pragma omp parallel for shared(nets) private(i)
        for(i = 0; i < nets.size(); ++i)
        {
            auto net = nets[i];
            auto & rc_tree_late = rc_trees_late[netlist.net_system().lookup(net)];
            flute.create_tree(placement, net, rc_tree_late, lib_late);

            auto & rc_tree_early = rc_trees_early[netlist.net_system().lookup(net)];
            flute.create_tree(placement, net, rc_tree_early, lib_early);
        }
#pragma omp barrier

    }
};


struct optimistic {
    using TimingType = boost::units::quantity< boost::units::si::time >;

    TimingType operator()(const TimingType &a, const TimingType &b) const {
        return std::min(a, b);
    }
    TimingType inverted(const TimingType &a, const TimingType &b) const {
        return std::max(a, b);
    }
    static double slack_signal() {
        return -1.0;
    }
    static TimingType best() {
        return std::numeric_limits<TimingType >::infinity();
    }
    static TimingType worst() {
        return -std::numeric_limits<TimingType >::infinity();
    }
};


struct pessimistic {
    using TimingType = boost::units::quantity< boost::units::si::time >;

    TimingType operator()(const TimingType &a, const TimingType &b) const {
        return std::max(a, b);
    }
    TimingType inverted(const TimingType &a, const TimingType &b) const {
        return std::min(a, b);
    }
    static double slack_signal() {
        return 1.0;
    }
    static TimingType best() {
        return -std::numeric_limits<TimingType >::infinity();
    }
    static TimingType worst() {
        return std::numeric_limits<TimingType >::infinity();
    }
};

TEST_CASE("simple generic sta", "[timing-driven_placement][generic_sta][flute][simple]")
{
    using namespace boost::units;
    using namespace boost::units::si;
    simple_moc simple;

    timing::graph_and_topology topology{simple.graph, simple.netlist, simple.lib_late};

    timing::timing_data late{simple.lib_late, simple.graph};
    timing::timing_data early{simple.lib_early, simple.graph};
    timing::test_calculator test{topology, early, late, quantity<si::time>(simple.dc.dc().clock.period*pico*seconds)};

//    using LateSTA = timing::generic_sta<timing::effective_capacitance_wire_model, pessimistic>;
//    using EarlySTA = timing::generic_sta<timing::effective_capacitance_wire_model, optimistic>;

    using LateSTA = timing::generic_sta<timing::lumped_capacitance_wire_model, pessimistic>;
    using EarlySTA = timing::generic_sta<timing::lumped_capacitance_wire_model, optimistic>;

    LateSTA late_sta(late, topology, simple.rc_trees_late); EarlySTA early_sta(early, topology, simple.rc_trees_early);
    late_sta.set_constraints(simple.dc.dc()); early_sta.set_constraints(simple.dc.dc());
    late_sta.update_ats(); early_sta.update_ats();
    test.compute_tests();
    late_sta.update_rts(); early_sta.update_rts();


    using boost::units::engineering_prefix;

    std::cout << boost::units::engineering_prefix;
    for(auto pin : simple.netlist.pin_system())
    {
        std::cout << "Timing Info. of Pin `" << simple.netlist.pin_name(pin.first) << "`" << std::endl;
        std::cout << "  LATE " << std::endl;
        std::cout << "     at R / F " << late_sta.rise_arrival(pin.first) << " / " << late_sta.fall_arrival(pin.first) << std::endl;
        std::cout << "     sw R / F " << late_sta.rise_slew(pin.first) << " / " << late_sta.fall_slew(pin.first) << std::endl;
        std::cout << "     sl R / F " << late_sta.rise_slack(pin.first) << " / " << late_sta.fall_slack(pin.first) << std::endl;

        std::cout << "  EARLY " << std::endl;
        std::cout << "     at R / F " << early_sta.rise_arrival(pin.first) << " / " << early_sta.fall_arrival(pin.first) << std::endl;
        std::cout << "     sw R / F " << early_sta.rise_slew(pin.first) << " / " << early_sta.fall_slew(pin.first) << std::endl;
        std::cout << "     sl R / F " << early_sta.rise_slack(pin.first) << " / " << early_sta.fall_slack(pin.first) << std::endl;
    }



}

