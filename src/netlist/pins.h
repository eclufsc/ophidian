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

#ifndef SRC_NETLIST_PINS_H_
#define SRC_NETLIST_PINS_H_

#include <vector>
#include "../entity/entity.h"
#include "../entity/vector_property.h"

namespace ophidian {
namespace netlist {

class pins {
	entity::system & m_system;

	entity::vector_property<std::string> m_names;
	entity::vector_property<entity::entity> m_owners;
	entity::vector_property<entity::entity> m_nets;
	entity::vector_property<entity::entity> m_std_cell_pin;

public:
	pins(entity::system & system);
	virtual ~pins();

	std::string name(entity::entity pin) const {
		return m_names[m_system.lookup(pin)];
	}
	entity::entity owner(entity::entity pin) const {
		return m_owners[m_system.lookup(pin)];
	}
	entity::entity net(entity::entity pin) const {
		return m_nets[m_system.lookup(pin)];
	}
	entity::entity standard_cell_pin(entity::entity pin) const {
		return m_std_cell_pin[m_system.lookup(pin)];
	}

	std::pair< std::vector<entity::entity>::const_iterator, std::vector<entity::entity>::const_iterator > owners() const {
		return std::make_pair(m_owners.begin(), m_owners.end());
	}

	std::pair< std::vector<entity::entity>::const_iterator, std::vector<entity::entity>::const_iterator > nets() const {
		return std::make_pair(m_nets.begin(), m_nets.end());
	}

	void name(entity::entity pin, std::string name);
	void owner(entity::entity pin, entity::entity owner);
	void net(entity::entity pin, entity::entity net);
	void standard_cell_pin(entity::entity pin, entity::entity std_cell_pin);

};

} /* namespace netlist */
} /* namespace ophidian */

#endif /* SRC_NETLIST_PINS_H_ */
