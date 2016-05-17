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

#ifndef SRC_STANDARD_CELL_CELLS_H_
#define SRC_STANDARD_CELL_CELLS_H_

#include "../entity_system/entity_system.h"
#include "../entity_system/vector_property.h"

namespace ophidian {
namespace standard_cell {

class cells {
        entity_system::entity_system & m_system;
        entity_system::vector_property<std::string> m_names;
    entity_system::vector_property<std::vector<entity_system::entity> > m_pins;
    entity_system::vector_property<bool> m_sequential;
public:
        cells(entity_system::entity_system & system);
	virtual ~cells();
        const entity_system::vector_property<std::string>& names() const {
		return m_names;
	}
        std::string name(entity_system::entity e) const {
		return m_names[m_system.lookup(e)];
	}
        const std::vector<entity_system::entity> & pins(entity_system::entity cell) const {
		return m_pins[m_system.lookup(cell)];
	}
    bool sequential(entity_system::entity cell) const {
        return m_sequential[m_system.lookup(cell)];
    }
        void name(entity_system::entity e, std::string name);
        void insert_pin(entity_system::entity cell, entity_system::entity pin);
    void pins(entity_system::entity cell, std::vector<entity_system::entity> pins);
    void sequential(entity_system::entity cell, bool sequential);
};

} /* namespace standard_cell */
} /* namespace ophidian */

#endif /* SRC_STANDARD_CELL_CELLS_H_ */
