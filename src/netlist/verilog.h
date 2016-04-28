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
