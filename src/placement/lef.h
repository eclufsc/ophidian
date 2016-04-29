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

#ifndef SRC_PLACEMENT_LEF_H_
#define SRC_PLACEMENT_LEF_H_

#include <istream>
#include <string>
#include <vector>
#include <standard_cells.h>
#include <floorplan.h>
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
void read_lef_site(std::istream &is, floorplan::floorplan * floorplan, library* lib);
void read_lef_layer(std::istream &is);
void read_lef_macro_pin(std::istream &is,  entity::entity cell, standard_cell::standard_cells * std_cells,
		library* lib);
void read_lef_macro(std::istream &is, standard_cell::standard_cells * std_cells,
		library* lib);
}

void read(std::istream& dot_lef, standard_cell::standard_cells* std_cells,
		library* lib, floorplan::floorplan * floorplan);

}
} /* namespace placement */
} /* namespace ophidian */

#endif /* SRC_PLACEMENT_LEF_H_ */
