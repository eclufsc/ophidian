/*
 * def.h
 *
 *  Created on: Feb 1, 2016
 *      Author: csguth
 */

#ifndef SRC_PLACEMENT_DEF_H_
#define SRC_PLACEMENT_DEF_H_

#include <istream>
#include "library.h"
#include "cells.h"

namespace openeda {
namespace placement {
namespace def {
namespace parser{
void get_next_token(std::istream &is, std::string &token,
		const char* beginComment);
void get_next_n_tokens(std::istream &is, std::vector<std::string> &tokens,
		const unsigned numTokens, const char* beginComment);


void read_init_def_components(std::istream &is, netlist::netlist* netlist,  library * lib,   cells* cells);
void read_def_pins(std::istream &is);
void read_def_nets(std::istream &is);

}

	void read(std::istream& input, netlist::netlist* netlist,  library*lib, cells * cells);


}
} /* namespace placement */
} /* namespace openeda */

#endif /* SRC_PLACEMENT_DEF_H_ */
