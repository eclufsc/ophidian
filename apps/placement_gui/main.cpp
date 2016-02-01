/*
 * main.cpp
 *
 *  Created on: Feb 1, 2016
 *      Author: csguth
 */

#include <iostream>
#include <string>

#include <fstream>
#include "netlist.h"
#include "verilog.h"

#include "library.h"
#include "lef.h"
#include "def.h"
#include "../placement/cells.h"


int main(int argc, char **argv) {

	if (argc != 4) {
		std::cerr << "Usage: " << argv[0]
				<< " <.lef file> <.v file> <.def file>" << std::endl;
		return -1;
	}

	std::ifstream dot_lef(argv[1], std::ifstream::in);
	std::ifstream dot_v(argv[2], std::ifstream::in);
	std::ifstream dot_def(argv[3], std::ifstream::in);

	if (!dot_def.good() || !dot_v.good() || !dot_def.good())
		std::cerr << "Error reading the files" << std::endl;

	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	openeda::netlist::verilog::read(dot_v, &netlist);
	dot_v.close();
	openeda::placement::library placement_library(&std_cells);
	openeda::placement::lef::read(dot_lef, &std_cells, &placement_library);
	dot_lef.close();
	openeda::placement::cells placement_cells(&netlist);
	openeda::placement::def::read(dot_def, &netlist, &placement_library,
			&placement_cells);
	dot_lef.close();

	for (auto c : netlist.cell_system()) {
		auto geometry = placement_library.geometry(
				netlist.cell_std_cell(c.second));
		placement_cells.geometry(c.second, geometry);
	}

	std::size_t count = 0;
	for (auto c : netlist.cell_system()) {
		if (count >= 100)
			break;
		auto position = placement_cells.position(c.second);
		std::cout << netlist.cell_name(c.second) << " "
				<< std_cells.name(netlist.cell_std_cell(c.second)) << " ("
				<< position.x() << ", " << position.y() << ")" << std::endl;
		++count;
	}
	return 0;
}

