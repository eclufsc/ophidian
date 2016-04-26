#include <iostream>

#include "../standard_cell/standard_cells.h"
#include "../placement/library.h"

#include "../placement/def.h"
#include "../placement/lef.h"
#include "../netlist/verilog.h"

double measure_HPWL(openeda::netlist::netlist& netlist, openeda::placement::placement &placement, openeda::placement::library &library) {
	double HPWL = 0.0;
	for(auto net : netlist.net_system())
	{
		auto net_pins = netlist.net_pins(net.second);
		if(net_pins.empty()) continue;
		std::vector<double> X(net_pins.size());
		std::vector<double> Y(net_pins.size());
		X.resize(0);
		Y.resize(0);
		for(auto pin : net_pins)
		{
			auto pos = placement.pin_position(pin);
			X.push_back(pos.x());
			Y.push_back(pos.y());
		}
		openeda::geometry::point<double> lower{*std::min_element(X.begin(), X.end()), *std::min_element(Y.begin(), Y.end())};
		openeda::geometry::point<double> upper{*std::max_element(X.begin(), X.end()), *std::max_element(Y.begin(), Y.end())};
		double local_HPWL = openeda::geometry::manhattan_distance(lower, upper);
		HPWL += local_HPWL;
	}
	HPWL /= static_cast<double>(library.dist2microns());
	return HPWL;
}

int main(int argc, char **argv) {

	if (argc != 4) {

		std::cerr << "invalid arguments." << std::endl;
		std::cerr << "usage: ./" << argv[0] << " <.v> <.def> <.lef>"
				<< std::endl;
		return -1;
	}

	openeda::standard_cell::standard_cells std_cell;
	openeda::netlist::netlist netlist { &std_cell };
	openeda::placement::library library { &std_cell };
	openeda::placement::placement placement { &netlist, &library };
	openeda::floorplan::floorplan floorplan;

	std::ifstream dot_v { argv[1], std::ifstream::in };
	std::ifstream dot_def { argv[2], std::ifstream::in };
	std::ifstream dot_lef { argv[3], std::ifstream::in };

	if (!dot_v.good()) {
		std::cerr << "error reading .v file" << std::endl;
		return -1;
	}

	if (!dot_def.good()) {
		std::cerr << "error reading .def file" << std::endl;
		return -1;
	}

	if (!dot_lef.good()) {
		std::cerr << "error reading .lef file" << std::endl;
		return -1;
	}

	openeda::netlist::verilog::read(dot_v, &netlist);
	openeda::placement::def::read(dot_def, &netlist, &placement, &floorplan);
	openeda::placement::lef::read(dot_lef, &std_cell, &library, &floorplan);

	double HPWL{measure_HPWL(netlist, placement, library)};
	std::cout << "HPWL before: " << HPWL << std::endl;

	for(std::size_t iteration = 1; iteration <= 10; ++iteration)
	{
	for(auto cell : netlist.cell_system())
	{
		auto cell_pins = netlist.cell_pins(cell.second);

		openeda::geometry::point<double> avg{0.0, 0.0};
		std::size_t count{0};
		for(auto pin : cell_pins)
		{
			auto pin_net = netlist.pin_net(pin);
			auto net_pins = netlist.net_pins(pin_net);
			for(auto pin2 : net_pins)
			{
				auto pin_position = placement.pin_position(pin2);
				avg.x( avg.x() + pin_position.x() );
				avg.y( avg.y() + pin_position.y() );
				++count;
			}
		}
		avg.x( avg.x() / static_cast<double>(count) );
		avg.y( avg.y() / static_cast<double>(count) );

		placement.cell_position(cell.second, avg);
	}
	HPWL = measure_HPWL(netlist, placement, library);
	std::cout << "HPWL iteration #" << iteration << " " << HPWL << std::endl;
	}

	std::cout << "HPWL after: " << HPWL << std::endl;
	std::cout << "Bye!" << std::endl;


	return 0;
}
