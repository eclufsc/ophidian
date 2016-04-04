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

#ifndef SRC_PLACEMENT_LEF_H_
#define SRC_PLACEMENT_LEF_H_

#include <istream>
#include <string>
#include <vector>
#include <standard_cells.h>
#include "library.h"
namespace ophidian {
namespace placement {
namespace lef {
namespace parser {

void get_next_token(std::istream &is, std::string &token,
		const char* beginComment);
void get_next_n_tokens(std::istream &is, std::vector<std::string> &tokens,
		const unsigned numTokens, const char* beginComment);

void read_lef_macro_site(std::istream &is);
void read_lef_site(std::istream &is);
void read_lef_layer(std::istream &is);
void read_lef_macro_pin(std::istream &is,  entity::entity cell, standard_cell::standard_cells * std_cells,
		library* lib);
void read_lef_macro(std::istream &is, standard_cell::standard_cells * std_cells,
		library* lib);
}

void read(std::istream& dot_lef, standard_cell::standard_cells* std_cells,
		library* lib);

}
} /* namespace placement */
} /* namespace ophidian */

#endif /* SRC_PLACEMENT_LEF_H_ */
