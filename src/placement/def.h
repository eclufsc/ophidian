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

#ifndef SRC_PLACEMENT_DEF_H_
#define SRC_PLACEMENT_DEF_H_

#include <istream>
#include "placement.h"

namespace ophidian {
namespace placement {
namespace def {
namespace parser {
void get_next_token(std::istream &is, std::string &token,
		const char* beginComment);
void get_next_n_tokens(std::istream &is, std::vector<std::string> &tokens,
		const unsigned numTokens, const char* beginComment);

void read_init_def_components(std::istream &is, netlist::netlist* netlist,
		placement* cells);
void read_def_pins(std::istream &is, netlist::netlist* netlist,
		placement* cells);
void read_def_nets(std::istream &is);

}

void read(std::istream& input, netlist::netlist* netlist, placement * cells);

}
} /* namespace placement */
} /* namespace openeda */

#endif /* SRC_PLACEMENT_DEF_H_ */
