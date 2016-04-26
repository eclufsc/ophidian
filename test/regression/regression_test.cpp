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

#include "../geometry/geometry.h"
#include "../netlist/verilog.h"
#include "../placement/def.h"
#include "../placement/lef.h"
#include "../placement/placement.h"

#include "../timing/endpoints.h"
#include "../timing/wns.h"
#include "../timing/liberty.h"
#include "../timing/graph_builder.h"
#include "../timing-driven_placement/flute_rc_tree_estimation.h"


#include "../timing/generic_sta.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include <omp.h>

double measure_HPWL(ophidian::netlist::netlist& netlist, ophidian::placement::placement &placement, ophidian::placement::library &library) {
    double HPWL = 0.0;
    for (auto net : netlist.net_system()) {
        auto net_pins = netlist.net_pins(net.second);
        if (net_pins.empty())
            continue;
        std::vector<double> X(net_pins.size());
        std::vector<double> Y(net_pins.size());
        X.resize(0);
        Y.resize(0);
        for (auto pin : net_pins) {
            auto pos = placement.pin_position(pin);
            X.push_back(pos.x());
            Y.push_back(pos.y());
        }
        ophidian::geometry::point<double> lower { *std::min_element(X.begin(), X.end()), *std::min_element(Y.begin(), Y.end()) };
        ophidian::geometry::point<double> upper { *std::max_element(X.begin(), X.end()), *std::max_element(Y.begin(), Y.end()) };
        double local_HPWL = ophidian::geometry::manhattan_distance(lower, upper);
        HPWL += local_HPWL;
    }
    HPWL /= static_cast<double>(library.dist2microns());
    return HPWL;
}

TEST_CASE("regression/hpwl", "[regression][hpwl]") {
    std::vector<std::string> circuits { "simple", "superblue16" };
    std::vector<double> golden_HPWL { 36.4645, 9.08702e+07 };
    for (std::size_t i = 0; i < circuits.size(); ++i) {
        std::ifstream dot_def("benchmarks/" + circuits.at(i) + "/" + circuits.at(i) + ".def", std::ifstream::in);
        std::ifstream dot_lef("benchmarks/" + circuits.at(i) + "/" + circuits.at(i) + ".lef", std::ifstream::in);
        std::ifstream dot_v("benchmarks/" + circuits.at(i) + "/" + circuits.at(i) + ".v", std::ifstream::in);
        REQUIRE(dot_def.good());
        REQUIRE(dot_lef.good());
        REQUIRE(dot_v.good());
        ophidian::standard_cell::standard_cells std_cells;
        ophidian::netlist::netlist netlist(&std_cells);
        ophidian::placement::library library(&std_cells);
        ophidian::placement::placement placement(&netlist, &library);
        ophidian::floorplan::floorplan floorplan;
        ophidian::placement::def::read(dot_def, &netlist, &placement, &floorplan);
        ophidian::placement::lef::read(dot_lef, &std_cells, &library, &floorplan);
        ophidian::netlist::verilog::read(dot_v, &netlist);
        double HPWL = measure_HPWL(netlist, placement, library);
        std::stringstream ss;
        ss.precision(5);
        ss << std::scientific << HPWL;
        std::stringstream ss_golden;
        ss_golden.precision(5);
        ss_golden << std::scientific << golden_HPWL.at(i);
        REQUIRE(ss.str() == ss.str());
    }
}

// STA
using namespace ophidian;
using namespace boost::units;
using namespace boost::units::si;



namespace moc {
struct golden {
    std::string name;
    double clk;
    double lwns;
    double ltns;
    double ewns;
    double etns;
};

struct circuit {
    standard_cell::standard_cells std_cells;
    netlist::netlist netlist{&std_cells};
    timing::library_timing_arcs tarcs{&std_cells};
    timing::library lib_late{&tarcs, &std_cells};
    timing::library lib_early{&tarcs, &std_cells};
    placement::library placement_lib{&std_cells};
    placement::placement placement{&netlist, &placement_lib};
    floorplan::floorplan floorplan;
    timing::graph graph;
    entity::vector_property< interconnection::rc_tree > rc_trees_early;
    entity::vector_property< interconnection::rc_tree > rc_trees_late;
    timing::design_constraints dc;

    circuit(const std::string & name, double clk)
    {
        std::ifstream dot_v("benchmarks/"+name+"/"+name+".v", std::ifstream::in);
        netlist::verilog::read(dot_v, &netlist);
#pragma omp critical
        {
        timing::liberty::read("benchmarks/"+name+"/"+name+"_Late.lib", lib_late);
        }
#pragma omp critical
        {
        timing::liberty::read("benchmarks/"+name+"/"+name+"_Early.lib", lib_early);
        }
        std::ifstream dot_def("benchmarks/"+name+"/"+name+".def", std::ifstream::in);
        placement::def::read(dot_def, &netlist, &placement, &floorplan);
        std::ifstream dot_lef("benchmarks/"+name+"/"+name+".lef", std::ifstream::in);
        placement::lef::read(dot_lef, &std_cells, &placement_lib, &floorplan);
        dc = timing::default_design_constraints{netlist}.dc();
        dc.clock.period = clk;
        for(auto driver : dc.input_drivers)
            std_cells.pin_direction(netlist.pin_std_cell(netlist.pin_by_name(driver.port_name)), standard_cell::pin_directions::OUTPUT);
        std_cells.pin_direction(netlist.pin_std_cell(netlist.pin_by_name(dc.clock.port_name)), standard_cell::pin_directions::OUTPUT);
        for(auto out_load : dc.output_loads)
        {
            auto PO_pin = netlist.pin_by_name(out_load.port_name);
            auto PO_std_cell_pin = netlist.pin_std_cell(PO_pin);
            std_cells.pin_direction(netlist.pin_std_cell(PO_pin), standard_cell::pin_directions::INPUT);
            lib_late.pin_capacitance(PO_std_cell_pin, boost::units::quantity<boost::units::si::capacitance>(out_load.pin_load*boost::units::si::femto*boost::units::si::farads));
            lib_early.pin_capacitance(PO_std_cell_pin, boost::units::quantity<boost::units::si::capacitance>(out_load.pin_load*boost::units::si::femto*boost::units::si::farads));

        }
        std::cout << "building the timing graph..." << std::endl << std::flush;
        timing::graph_builder::build(netlist, lib_late, dc, graph);
        std::cout << "building the timing graph DONE" << std::endl << std::flush;
        netlist.register_net_property(&rc_trees_early);
        netlist.register_net_property(&rc_trees_late);
        std::vector<entity::entity> nets(netlist.net_system().size());
        nets.resize(0);
        for(auto net : netlist.net_system())
            nets.push_back(net.first);

        std::size_t i;
        timingdriven_placement::flute_rc_tree_creator flute;
        std::cout << "    creating rc_trees..." << std::endl << std::flush;
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
        std::cout << "    creating rc_trees DONE!!" << std::endl << std::flush;
    }
    virtual ~circuit() {
    }


};

}


TEST_CASE("regression/iccad 2015 sta", "[regression][sta][flute]") {

    using TimeType = quantity< si::time > ;

    std::cout << "STATIC TIMING ANALYSIS REGRESSION TEST..." << std::endl << std::flush;

    std::cout << "   USING ICCAD 2015 ITDP CONTEST'S BENCHMARKS" << std::endl << std::flush;

    std::vector<moc::golden> golden_circuits{
        {"simple", 80.0, -103.776, -124.11, 0.0, 0.0}
        ,{"superblue18", 7000.0, -4550.7, -1.0348e+06, -19.0054, -282.999}
        ,{"superblue16", 5500.0, -4581.07, -776037, -10.6536, -113.749}
        ,{"superblue4", 6000.0, -6220.67, -3.47669e+06, -12.5521, -519.386}
        ,{"superblue1", 9500.0, -4976.58, -459744, -9.34402, -317.437}
        ,{"superblue3", 10000.0, -10145.9, -1.50283e+06, -78.3588, -1458.78}
        ,{"superblue5", 9000.0, -25702.9, -6.96515e+06, -36.7694, -591.422}
        ,{"superblue10", 10000.0, -16486.9, -3.31528e+07, -8.61971, -620.952}
        ,{"superblue7", 5500.0, -15216.3, -1.85738e+06, -7.64567, -1985.85}
    };


    omp_set_num_threads(4);

    std::vector<std::pair<int, int> > ranges{
        {0,3},
        {4,6},
        {7,7},
        {8,8}
    };


    for(auto range : ranges)
    {
#pragma omp parallel for
    for(std::size_t i = range.first; i <= range.second; ++i)
    {

        const moc::golden & golden = golden_circuits[i];
#pragma omp critical
        {
        std::cout << "   CIRCUIT " << golden.name <<  " running on thread " << omp_get_thread_num() <<  std::endl << std::flush;
        }
        const std::string & ckt = golden.name;
        moc::circuit moc(ckt,  golden.clk);
        timing::graph_and_topology topology{moc.graph, moc.netlist, moc.lib_late};

        timing::timing_data late{moc.lib_late, moc.graph};
        timing::timing_data early{moc.lib_early, moc.graph};
        timing::test_calculator test{topology, early, late, quantity<si::time>(moc.dc.clock.period*pico*seconds)};
        using LateSTA = timing::generic_sta<timing::lumped_capacitance_wire_model, timing::pessimistic>;
        using EarlySTA = timing::generic_sta<timing::lumped_capacitance_wire_model, timing::optimistic>;

        LateSTA late_sta(late, topology, moc.rc_trees_late); EarlySTA early_sta(early, topology, moc.rc_trees_early);
        late_sta.set_constraints(moc.dc); early_sta.set_constraints(moc.dc);
        late_sta.update_ats();
        early_sta.update_ats();
        test.compute_tests();
        late_sta.update_rts();
        early_sta.update_rts();

        timing::endpoints endpoints(moc.netlist);
        timing::wns WNS_late(endpoints, late_sta);
        double lWNS = WNS_late.value().value();
        timing::wns WNS_early(endpoints, early_sta);
        double eWNS = WNS_early.value().value();
        timing::wns TNS_late(endpoints, late_sta);
        double lTNS = TNS_late.value().value();
        timing::wns TNS_early(endpoints, early_sta);
        double eTNS = TNS_early.value().value();

        CHECK( golden.lwns*1e-12 == Approx(lWNS) );
        CHECK( golden.ewns*1e-12 == Approx(eWNS) );
        CHECK( golden.ltns*1e-12 == Approx(lTNS) );
        CHECK( golden.etns*1e-12 == Approx(eTNS) );


#pragma omp critical
        {
        std::cout << "   CIRCUIT " << golden.name << " done  on thread " << omp_get_thread_num() << std::endl << std::flush;
        std::cout << std::endl << std::flush;
        std::cout << std::endl << std::flush;
        std::cout << std::endl << std::flush;
        std::cout << std::endl << std::flush;
        }
    }
    }

}
