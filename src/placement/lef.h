/*
 * lef.h
 *
 *  Created on: Jan 31, 2016
 *      Author: csguth
 */

#ifndef SRC_PLACEMENT_LEF_H_
#define SRC_PLACEMENT_LEF_H_

#include <istream>
#include <string>
#include <vector>
#include <standard_cells.h>
#include "library.h"
namespace openeda {
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
} /* namespace openeda */

#endif /* SRC_PLACEMENT_LEF_H_ */
