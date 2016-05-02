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

#ifndef SRC_NETLIST_CELLS_H_
#define SRC_NETLIST_CELLS_H_

#include <vector>
#include "../entity/vector_property.h"
#include "../standard_cell/standard_cells.h"

namespace ophidian {
namespace netlist {

class cells {
	entity::system & m_system;

	entity::vector_property<std::string> m_names;
	entity::vector_property<entity::entity> m_standard_cells;
	entity::vector_property<std::vector<entity::entity> > m_pins;
public:
	cells(entity::system & system);
	virtual ~cells();

	std::string name(entity::entity cell) const {
		return m_names[m_system.lookup(cell)];
	}
	entity::entity standard_cell(entity::entity cell) const {
		return m_standard_cells[m_system.lookup(cell)];
	}
    const std::vector<entity::entity> & pins(entity::entity cell) const {
		return m_pins[m_system.lookup(cell)];
	}

	std::pair< std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator > names() const {
		return std::make_pair(m_names.begin(), m_names.end());
	}

	std::pair< std::vector<entity::entity>::const_iterator, std::vector<entity::entity>::const_iterator > standard_cells() const {
		return std::make_pair(m_standard_cells.begin(), m_standard_cells.end());
	}

	std::pair< std::vector<std::vector<entity::entity>>::const_iterator, std::vector<std::vector<entity::entity>>::const_iterator > pins() const {
		return std::make_pair(m_pins.begin(), m_pins.end());
	}

	void insert_pin(entity::entity cell, entity::entity pin);
	void pins(entity::entity cell, std::vector<entity::entity> pins);
	void name(entity::entity cell, std::string name);
	void standard_cell(entity::entity cell, entity::entity std_cell);


    void pins_preallocate(entity::entity cell, std::size_t pin_count);

};

} /* namespace netlist */
} /* namespace ophidian */

#endif /* SRC_NETLIST_CELLS_H_ */
