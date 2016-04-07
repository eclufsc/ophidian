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


using namespace ophidian;


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


void run(std::string benchmark) {
    standard_cell::standard_cells std_cells;
    netlist::netlist netlist{&std_cells};

    timing::library_timing_arcs tarcs{&std_cells};
    timing::library lib(&tarcs, &std_cells);
    std::ifstream dot_v("benchmarks/"+benchmark+"/"+benchmark+".v", std::ifstream::in);
    REQUIRE(dot_v.good());
    netlist::verilog::read(dot_v, &netlist);
    timing::liberty::read("benchmarks/"+benchmark+"/"+benchmark+"_Late.lib", lib);
    placement::library placement_lib{&std_cells};
    placement::placement placement{&netlist, &placement_lib};

    std::ifstream dot_def("benchmarks/"+benchmark+"/"+benchmark+".def", std::ifstream::in);
    REQUIRE(dot_def.good());

    placement::def::read(dot_def, &netlist, &placement);


    std::ifstream dot_lef("benchmarks/"+benchmark+"/"+benchmark+".lef", std::ifstream::in);
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


    std::cout << "creating STA" << std::endl;
    timing::generic_sta<timing::ceff, pessimistic> sta(netlist, lib, graph, rc_trees);
    sta.set_constraints(dc.dc());

    std::cout << "creating RC_trees" << std::endl;
    timingdriven_placement::flute_rc_tree_creator flute;

    std::vector<entity::entity> nets(netlist.net_system().size());
    nets.resize(0);
    for(auto net : netlist.net_system())
        nets.push_back(net.first);

    std::size_t i;
#pragma omp parallel for shared(nets) private(i)
    for(i = 0; i < nets.size(); ++i)
    {
        auto net = nets[i];
        auto & rc_tree = rc_trees[netlist.net_system().lookup(net)];
        flute.create_tree(placement, net, rc_tree, lib);
    }
#pragma omp barrier

    std::cout << "running STA" << std::endl;
    sta.run();

    using TimingType = boost::units::quantity< boost::units::si::time > ;
    TimingType WNS = std::numeric_limits<TimingType>::infinity();
    TimingType TNS;
    for(auto PO = netlist.PO_begin(); PO != netlist.PO_end(); ++PO)
    {
        WNS = std::min( WNS, std::min(sta.rise_slack(*PO), sta.fall_slack(*PO)));
        TNS += std::min(0.0*boost::units::si::seconds, std::min( WNS, std::min(sta.rise_slack(*PO), sta.fall_slack(*PO))));
    }

    std::cout << "WNS " << WNS << std::endl;
    std::cout << "TNS " << TNS << std::endl;

    for(auto pin : netlist.pin_system())
    {
        std::cout << netlist.pin_name(pin.first) << " ";
        std::cout << "at r " << sta.rise_arrival(pin.first) << " ";
        std::cout << "at f " << sta.fall_arrival(pin.first) << " ";
        std::cout << "slew r " << sta.rise_slew(pin.first) << " ";
        std::cout << "slew f " << sta.fall_slew(pin.first) << " ";
        std::cout << "slack r " << sta.rise_slack(pin.first) << " ";
        std::cout << "slack f " << sta.fall_slack(pin.first) << " ";
        std::cout << std::endl;
    }
}

TEST_CASE("generic sta simple", "[timing][generic_sta][simple]")
{
    omp_set_num_threads(8);
    run("simple");
}

TEST_CASE("generic sta superblue16", "[timing][generic_sta][superblue16]")
{
    omp_set_num_threads(8);
    run("superblue16");
}




TEST_CASE("optimistic sta", "[timing][generic_sta][optimistic]")
{
    std::string benchmark = "simple";

    standard_cell::standard_cells std_cells;
    netlist::netlist netlist{&std_cells};

    timing::library_timing_arcs tarcs{&std_cells};
    timing::library lib(&tarcs, &std_cells);
    std::ifstream dot_v("benchmarks/"+benchmark+"/"+benchmark+".v", std::ifstream::in);
    REQUIRE(dot_v.good());
    netlist::verilog::read(dot_v, &netlist);
    timing::liberty::read("benchmarks/"+benchmark+"/"+benchmark+"_Early.lib", lib);
    placement::library placement_lib{&std_cells};
    placement::placement placement{&netlist, &placement_lib};

    std::ifstream dot_def("benchmarks/"+benchmark+"/"+benchmark+".def", std::ifstream::in);
    REQUIRE(dot_def.good());

    placement::def::read(dot_def, &netlist, &placement);


    std::ifstream dot_lef("benchmarks/"+benchmark+"/"+benchmark+".lef", std::ifstream::in);
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


    std::cout << "creating STA" << std::endl;
    timing::generic_sta<timing::ceff, optimistic> sta(netlist, lib, graph, rc_trees);
    sta.set_constraints(dc.dc());

    std::cout << "creating RC_trees" << std::endl;
    timingdriven_placement::flute_rc_tree_creator flute;

    std::vector<entity::entity> nets(netlist.net_system().size());
    nets.resize(0);
    for(auto net : netlist.net_system())
        nets.push_back(net.first);

    std::size_t i;
#pragma omp parallel for shared(nets) private(i)
    for(i = 0; i < nets.size(); ++i)
    {
        auto net = nets[i];
        auto & rc_tree = rc_trees[netlist.net_system().lookup(net)];
        flute.create_tree(placement, net, rc_tree, lib);
    }
#pragma omp barrier

    std::cout << "running STA" << std::endl;
    sta.run();

    using TimingType = boost::units::quantity< boost::units::si::time > ;
    TimingType WNS = std::numeric_limits<TimingType>::infinity();
    TimingType TNS;
    for(auto PO = netlist.PO_begin(); PO != netlist.PO_end(); ++PO)
    {
        WNS = std::min( WNS, std::min(sta.rise_slack(*PO), sta.fall_slack(*PO)));
        TNS += std::min(0.0*boost::units::si::seconds, std::min( WNS, std::min(sta.rise_slack(*PO), sta.fall_slack(*PO))));
    }

    std::cout << "WNS " << WNS << std::endl;
    std::cout << "TNS " << TNS << std::endl;

    for(auto pin : netlist.pin_system())
    {
        std::cout << netlist.pin_name(pin.first) << " ";
        std::cout << "at r " << sta.rise_arrival(pin.first) << " ";
        std::cout << "at f " << sta.fall_arrival(pin.first) << " ";
        std::cout << "slew r " << sta.rise_slew(pin.first) << " ";
        std::cout << "slew f " << sta.fall_slew(pin.first) << " ";
        std::cout << "slack r " << sta.rise_slack(pin.first) << " ";
        std::cout << "slack f " << sta.fall_slack(pin.first) << " ";
        std::cout << std::endl;
    }




}
