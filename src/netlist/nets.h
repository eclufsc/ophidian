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

#ifndef SRC_NETLIST_NETS_H_
#define SRC_NETLIST_NETS_H_

#include <vector>
#include "../entity/entity.h"
#include "../entity/vector_property.h"
#include <utility>

namespace ophidian {
namespace netlist {

class nets {
	entity::system & m_system;

	entity::vector_property<std::vector<entity::entity>> m_pins;
	entity::vector_property<std::string> m_names;

public:
	nets(entity::system & system);
	virtual ~nets();

	std::string name(entity::entity net) const {
		return m_names[m_system.lookup(net)];
	}
	std::vector<entity::entity> pins(entity::entity net) const {
		return m_pins[m_system.lookup(net)];
	}
	std::pair< std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator > names() const {
		return std::make_pair(m_names.begin(), m_names.end());
	}

	std::pair< std::vector<std::vector<entity::entity>>::const_iterator, std::vector<std::vector<entity::entity>>::const_iterator > pins() const {
		return std::make_pair(m_pins.begin(), m_pins.end());
	}

	void connect(entity::entity net, entity::entity pin);
	void disconnect(entity::entity net, entity::entity pin);
	void pins(entity::entity net, std::vector<entity::entity> pins);
	void name(entity::entity net, std::string name);
};

} /* namespace netlist */
} /* namespace ophidian */

#endif /* SRC_NETLIST_NETS_H_ */
