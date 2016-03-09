/*
 * verilog.cpp
 *
 *  Created on: Jan 30, 2016
 *      Author: csguth
 */

#include <verilog.h>
#include <iostream>
namespace openeda {
namespace netlist {
namespace verilog {
namespace parser {

/* generic helper functions */
bool is_special_char(char c) {
	static const char specialChars[] = { '(', ')', ',', ':', ';', '/', '#', '[',
			']', '{', '}', '*', '\"', '\\' };

	for (unsigned i = 0; i < sizeof(specialChars); ++i) {
		if (c == specialChars[i])
			return true;
	}

	return false;
}

std::vector<std::string> tokenize(std::string line) {
	std::vector<std::string> tokens;
	std::string last_token;
	for (auto c : line) {
		if (isspace(c) || is_special_char(c)) {
			if (!last_token.empty()) {
				tokens.push_back(last_token);
				last_token.clear();
			}
		} else
			last_token.push_back(c);
	}
	if (!last_token.empty())
		tokens.push_back(last_token);
	return tokens;
}

}

void read(std::istream & in, netlist* netlist) {
	std::cout << "reading .v file..." << std::endl;
	std::string line;
	std::getline(in, line);
	auto tokens = parser::tokenize(line);
	assert(tokens[0] == "module");
	netlist->module_name(tokens[1]);
	do {
		std::getline(in, line);
		tokens = parser::tokenize(line);
	} while (tokens.size() != 2 || tokens[0] != "Start" || tokens[1] != "PIs");

	do {
		std::getline(in, line);
		tokens = parser::tokenize(line);
		if (tokens.size() == 2 && tokens[0] == "input")
			netlist->connect(netlist->net_insert(tokens[1]), netlist->PI_insert(tokens[1]));
	} while (tokens.size() != 2 || tokens[0] != "Start" || tokens[1] != "POs");

	do {
		std::getline(in, line);
		tokens = parser::tokenize(line);
		if (tokens.size() == 2 && tokens[0] == "output")
			netlist->connect(netlist->net_insert(tokens[1]), netlist->PO_insert(tokens[1]));
	} while (tokens.size() != 2 || tokens[0] != "Start" || tokens[1] != "wires");

	do {
		std::getline(in, line);
		tokens = parser::tokenize(line);
		if (tokens.size() == 2 && tokens[0] == "wire")
			netlist->net_insert(tokens[1]);
	} while (tokens.size() != 2 || tokens[0] != "Start" || tokens[1] != "cells");

	do {
		std::getline(in, line);
		tokens = parser::tokenize(line);
		if(tokens.empty())
			continue;
		if(tokens.size() == 1 && tokens[0] == "endmodule")
			break;
		std::string cell_type = tokens[0];
		std::string cell_name = tokens[1];
		for(std::size_t i = 2; i < tokens.size()-1; i+=2)
			netlist->connect(netlist->net_insert(tokens[i+1]),
					netlist->pin_insert(netlist->cell_insert(cell_name, cell_type), tokens[i].substr(1)));
	} while (tokens.size() != 1 || tokens[0] != "endmodule");
	std::cout << "reading .v file DONE" << std::endl;
}

void write(std::ostream & out, standard_cell::standard_cells * std_cells,
		netlist* netlist) {
	out << "module simple (" << std::endl;
	std::vector<entity::entity> pads;
	pads.insert(pads.end(), netlist->PI_begin(), netlist->PI_end());
	pads.insert(pads.end(), netlist->PO_begin(), netlist->PO_end());
	std::size_t i = 0;
	for (auto pad : pads) {
		out << netlist->pin_name(pad);
		if (i < pads.size() - 1)
			out << ",";
		out << std::endl;
		++i;
	}
	out << ");" << std::endl << std::endl;
	out << "// Start PIs" << std::endl;
	for (auto it = netlist->PI_begin(); it != netlist->PI_end(); ++it)
		out << "input " << netlist->pin_name(*it) << ";" << std::endl;
	out << std::endl;
	out << "// Start POs" << std::endl;
	for (auto it = netlist->PO_begin(); it != netlist->PO_end(); ++it)
		out << "output " << netlist->pin_name(*it) << ";" << std::endl;
	out << std::endl;
	out << "// Start wires" << std::endl;
	for (auto it = netlist->net_names().first;
			it != netlist->net_names().second; ++it)
		out << "wire " << *it << ";" << std::endl;
	out << std::endl;
	out << "// Start cells" << std::endl;
	for (auto cell : netlist->cell_system().entities()) {
		auto std_cell = netlist->cell_std_cell(cell.first);
		out << std_cells->cell_name(std_cell) << " ";
		out << netlist->cell_name(cell.first) << " ( ";
		auto cell_pins = netlist->cell_pins(cell.first);
		bool first = true;
		for (auto pin : cell_pins) {
			auto net = netlist->pin_net(pin);
			if (!first)
				out << ", ";
			out << "."
					<< netlist->pin_name(pin).substr(
							netlist->pin_name(pin).find(":") + 1) << "("
					<< netlist->net_name(net) << ")";
			first = false;
		}
		out << " );" << std::endl;

	}
	out << std::endl;
	out << "endmodule" << std::endl;
}

}
} /* namespace netlist */
} /* namespace openeda */

