#include "../catch.hpp"

#include "../geometry/geometry.h"
#include "../netlist/verilog.h"
#include "../placement/def.h"
#include "../placement/lef.h"
#include "../placement/placement.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

double measure_HPWL(openeda::netlist::netlist& netlist, openeda::placement::placement &placement, openeda::placement::library &library) {
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
		openeda::geometry::point<double> lower { *std::min_element(X.begin(), X.end()), *std::min_element(Y.begin(), Y.end()) };
		openeda::geometry::point<double> upper { *std::max_element(X.begin(), X.end()), *std::max_element(Y.begin(), Y.end()) };
		double local_HPWL = openeda::geometry::manhattan_distance(lower, upper);
		HPWL += local_HPWL;
	}
	HPWL /= static_cast<double>(library.dist2microns());
	return HPWL;
}

TEST_CASE("regression/hpwl simple", "[regression][hpwl]") {
	std::vector<std::string> circuits { "simple", "superblue16" };
	std::vector<double> golden_HPWL { 36.4645, 9.08702e+07 };
	for (std::size_t i = 0; i < circuits.size(); ++i) {
		std::ifstream dot_def("benchmarks/" + circuits.at(i) + "/" + circuits.at(i) + ".def", std::ifstream::in);
		std::ifstream dot_lef("benchmarks/" + circuits.at(i) + "/" + circuits.at(i) + ".lef", std::ifstream::in);
		std::ifstream dot_v("benchmarks/" + circuits.at(i) + "/" + circuits.at(i) + ".v", std::ifstream::in);
		REQUIRE(dot_def.good());
		REQUIRE(dot_lef.good());
		REQUIRE(dot_v.good());
		openeda::standard_cell::standard_cells std_cells;
		openeda::netlist::netlist netlist(&std_cells);
		openeda::placement::library library(&std_cells);
		openeda::placement::placement placement(&netlist, &library);
		openeda::placement::def::read(dot_def, &netlist, &placement);
		openeda::placement::lef::read(dot_lef, &std_cells, &library);
		openeda::netlist::verilog::read(dot_v, &netlist);
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

#include "../timing/static_timing_analysis.h"
#include "../timing/liberty.h"
#include "../timing/graph_builder.h"
#include "../timing-driven_placement/sta_flute_net_calculator.h"
#include "../timing/simple_design_constraint.h"


#include <boost/date_time/posix_time/posix_time.hpp>

#include <omp.h>

TEST_CASE("regression/ simple flute STA", "[regression][sta][flute]") {
	using namespace openeda;
    std::vector<std::string> circuits { "simple", "superblue16" };
//        std::vector<std::string> circuits { "simple" };
	for (std::size_t i { 0 }; i < circuits.size(); ++i) {

        std::cout << "reading inputs..." << std::endl;
        boost::posix_time::ptime mst1 = boost::posix_time::microsec_clock::local_time();
		std::ifstream dot_def("benchmarks/" + circuits.at(i) + "/" + circuits.at(i) + ".def", std::ifstream::in);
		std::ifstream dot_lef("benchmarks/" + circuits.at(i) + "/" + circuits.at(i) + ".lef", std::ifstream::in);
		std::ifstream dot_v("benchmarks/" + circuits.at(i) + "/" + circuits.at(i) + ".v", std::ifstream::in);
		REQUIRE(dot_def.good());
		REQUIRE(dot_lef.good());
		REQUIRE(dot_v.good());
		standard_cell::standard_cells std_cells;
		netlist::netlist netlist(&std_cells);
		placement::library library(&std_cells);
		placement::placement placement(&netlist, &library);
		placement::def::read(dot_def, &netlist, &placement);
		placement::lef::read(dot_lef, &std_cells, &library);
		netlist::verilog::read(dot_v, &netlist);

//        for(auto pin : netlist.pin_system())
//        {
//            std::cout << pin.first.id() << " = " << netlist.pin_name(pin.first) << std::endl;
//        }


		timing::library_timing_arcs timing_arcs { &std_cells };
		timing::library timing_lib { &timing_arcs, &std_cells };
		timing::liberty::read("benchmarks/" + circuits.at(i) + "/" + circuits.at(i) + "_Late.lib", timing_lib);
        boost::posix_time::ptime mst2 = boost::posix_time::microsec_clock::local_time();
        boost::posix_time::time_duration msdiff = mst2 - mst1;
        std::cout << "inputs read in " << msdiff.total_milliseconds()/1000.0 << " seconds" << std::endl;


        std::cout << "Building graph..." << std::endl;
        mst1 = boost::posix_time::microsec_clock::local_time();
		timing::graph graph;
        timing::default_design_constraints default_dc(netlist);
        timing::design_constraints dc( default_dc.dc() );

        for(auto driver : dc.input_drivers)
            std_cells.pin_direction(netlist.pin_std_cell(netlist.pin_by_name(driver.port_name)), standard_cell::pin_directions::OUTPUT);

        std_cells.pin_direction(netlist.pin_std_cell(netlist.pin_by_name(dc.clock.port_name)), standard_cell::pin_directions::OUTPUT);

        for(auto out_load : dc.output_loads)
		{
			auto PO_pin = netlist.pin_by_name(out_load.port_name);
			auto PO_std_cell_pin = netlist.pin_std_cell(PO_pin);
            std_cells.pin_direction(netlist.pin_std_cell(PO_pin), standard_cell::pin_directions::INPUT);
			timing_lib.pin_capacitance(PO_std_cell_pin, boost::units::quantity<boost::units::si::capacitance>(out_load.pin_load*boost::units::si::femto*boost::units::si::farads));
		}
        timing::graph_builder::build(netlist, timing_lib, dc, graph);
        mst2 = boost::posix_time::microsec_clock::local_time();
        msdiff = mst2 - mst1;
        std::cout << "Building graph DONE in " << msdiff.total_milliseconds()/1000.0 << " seconds" << std::endl;

        std::cout << "running STA... " << std::endl;
        timingdriven_placement::sta_flute_net_calculator flute(graph, placement, timing_lib, netlist);
		timing::static_timing_analysis STA(graph, timing_lib, &flute);
		for(auto net : netlist.net_system())
			STA.make_dirty(net.first);
        STA.set_constraints(netlist, dc);
        omp_set_num_threads(8);
        mst1 = boost::posix_time::microsec_clock::local_time();
		STA.run();
        mst2 = boost::posix_time::microsec_clock::local_time();
        msdiff = mst2 - mst1;
        std::cout << "STA DONE in " << msdiff.total_milliseconds()/1000.0 << " seconds" << std::endl;




//        for(auto pin : netlist.pin_system())
//        {
//            std::cout << netlist.pin_name(pin.first) << " ";
//            std::cout << "at r " << STA.rise_arrival(pin.first) << " ";
//            std::cout << "at f " << STA.fall_arrival(pin.first) << " ";
//            std::cout << "slew r " << STA.rise_slew(pin.first) << " ";
//            std::cout << "slew f " << STA.fall_slew(pin.first) << " ";
//            std::cout << "slack r " << STA.rise_slack(pin.first) << " ";
//            std::cout << "slack f " << STA.fall_slack(pin.first) << " ";
//            std::cout << std::endl;
//        }



	}
}
