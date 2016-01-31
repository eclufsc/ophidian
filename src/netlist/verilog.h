/*
 * verilog.h
 *
 *  Created on: Jan 30, 2016
 *      Author: csguth
 */

#ifndef SRC_NETLIST_VERILOG_H_
#define SRC_NETLIST_VERILOG_H_

#include "netlist.h"
#include <fstream>

namespace openeda {
namespace netlist {

namespace verilog {
namespace parser {
bool is_special_char(char c);
std::vector<std::string> tokenize(std::string line);
}

void read(std::istream & in, netlist* netlist);
void write(std::ostream & out, standard_cell::standard_cells * std_cells,
		netlist* netlist);
}

} /* namespace netlist */
} /* namespace openeda */

#endif /* SRC_NETLIST_VERILOG_H_ */
