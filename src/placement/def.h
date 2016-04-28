/*
 * Copyright 2016 Ophidian
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
 */

#ifndef SRC_PLACEMENT_DEF_H_
#define SRC_PLACEMENT_DEF_H_

#include <istream>
#include "placement.h"
#include "floorplan.h"

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

void read(std::istream& input, netlist::netlist* netlist, placement * cells, floorplan::floorplan * floorplan);

}
} /* namespace placement */
} /* namespace ophidian */

#endif /* SRC_PLACEMENT_DEF_H_ */
