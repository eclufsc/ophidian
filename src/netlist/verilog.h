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

#ifndef SRC_NETLIST_VERILOG_H_
#define SRC_NETLIST_VERILOG_H_

#include "netlist.h"
#include <fstream>

namespace ophidian {
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
} /* namespace ophidian */

#endif /* SRC_NETLIST_VERILOG_H_ */
