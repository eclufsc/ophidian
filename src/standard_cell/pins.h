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

#ifndef SRC_STANDARD_CELL_PINS_H_
#define SRC_STANDARD_CELL_PINS_H_

#include "../entity_system/entity_system.h"
#include "../entity_system/vector_property.h"

namespace ophidian {
namespace standard_cell {


enum class pin_directions {
    NOT_ASSIGNED, INPUT, OUTPUT
};

class pins {
    entity_system::entity_system & m_system;

    entity_system::vector_property< std::string > m_names;
    entity_system::vector_property< entity_system::entity > m_owners;
    entity_system::vector_property< pin_directions > m_directions;
    entity_system::vector_property< bool > m_clock_input;

public:
    pins(entity_system::entity_system & system);
    virtual ~pins();

    void name(entity_system::entity pin, std::string name);
    std::string name(entity_system::entity pin) const {
        return m_names[m_system.lookup(pin)];
    }
    void owner(entity_system::entity pin, entity_system::entity cell);
    entity_system::entity owner(entity_system::entity pin) const {
        return m_owners[m_system.lookup(pin)];
    }

    void direction(entity_system::entity pin, pin_directions dir);
    pin_directions direction(entity_system::entity pin) const {
        return m_directions[m_system.lookup(pin)];
    }


    void clock_input(entity_system::entity pin, bool clock);
    bool clock_input(entity_system::entity pin) const {
        return m_clock_input[m_system.lookup(pin)];
    }

};

} /* namespace standard_cell */
} /* namespace ophidian */

#endif /* SRC_STANDARD_CELL_PINS_H_ */
