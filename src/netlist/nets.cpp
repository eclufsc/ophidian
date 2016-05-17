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

#include "nets.h"

namespace ophidian {
namespace netlist {

nets::nets(entity_system::entity_system & system) : m_system(system) {
	m_system.register_property(&m_names);
	m_system.register_property(&m_pins);
}

nets::~nets() {
}

void nets::connect(entity_system::entity net, entity_system::entity pin) {
	m_pins[m_system.lookup(net)].push_back(pin);
}

void nets::disconnect(entity_system::entity net, entity_system::entity pin) {
	auto & pins_vector = m_pins[m_system.lookup(net)];
	std::size_t i = 0;
	while(i < pins_vector.size())
	{
		if(pins_vector[i] == pin)
		{
			pins_vector[i] = pins_vector.back();
			pins_vector.pop_back();
			return;
		}
		++i;
	}
	throw std::runtime_error("pin already disconnected");
}

void nets::pins(entity_system::entity net, std::vector<entity_system::entity> pins) {
	m_pins[m_system.lookup(net)] = pins;
}

void nets::name(entity_system::entity net, std::string name) {
    m_names[m_system.lookup(net)] = name;
}

void nets::preallocate_pins(entity_system::entity net, std::size_t pin_count)
{
    m_pins[m_system.lookup(net)].reserve(pin_count);
}


} /* namespace netlist */
} /* namespace ophidian */
