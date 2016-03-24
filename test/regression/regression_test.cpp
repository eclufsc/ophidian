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
#include "../timing/graph.h"
TEST_CASE("regression/ simple flute STA", "[regression][sta][flute]") {
	using namespace openeda;
	std::vector<std::string> circuits { "simple" };
	for (std::size_t i { 0 }; i < circuits.size(); ++i) {
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
		timing::library_timing_arcs timing_arcs { &std_cells };
		timing::library timing_lib { &timing_arcs, &std_cells };
		timing::liberty::read("benchmarks/" + circuits.at(i) + "/" + circuits.at(i) + "_Late.lib", timing_lib);

	}
}
