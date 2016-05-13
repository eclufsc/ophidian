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

#ifndef SRC_TIMING_LIBRARY_TIMING_ARCS_H_
#define SRC_TIMING_LIBRARY_TIMING_ARCS_H_

#include "../entity_system/entity_system.h"
#include "../standard_cell/standard_cells.h"
#include "lookup_table.h"
#include <unordered_map>

namespace ophidian {
namespace timing {


// Only for pairs of std::hash-able types for simplicity.
// You can of course template this struct to allow other hash functions
//struct pair_hash {
//    template <class T1, class T2>
//    std::size_t operator () (const std::pair<T1,T2> &p) const {
//        auto h1 = std::hash<T1>{}(p.first);
//        auto h2 = std::hash<T2>{}(p.second);

//        // Mainly for demonstration purposes, i.e. works but is overly simple
//        // In the real world, use sth. like boost.hash_combine
//        return h1 ^ h2;
//    }
//};

struct pair_entity_hash {
    std::size_t operator () (const std::pair<entity_system::entity,entity_system::entity> &p) const {
        return std::hash<uint32_t>{}(p.first) ^ std::hash<uint32_t>{}(p.second);
    }
};

class library_timing_arcs {
public:
private:
	standard_cell::standard_cells & m_std_cells;

        entity_system::entity_system m_system;
        entity_system::vector_property<entity_system::entity> m_from;
        entity_system::vector_property<entity_system::entity> m_to;

	//	// std_cell properties
                entity_system::vector_property< std::vector<entity_system::entity> > m_pin_timing_arcs;


        std::unordered_map< std::pair< entity_system::entity, entity_system::entity >, entity_system::entity,pair_entity_hash > m_pinpair2arc;
public:
	library_timing_arcs(standard_cell::standard_cells * std_cells);
	virtual ~library_timing_arcs();
        entity_system::entity create(entity_system::entity from, entity_system::entity to);
        entity_system::entity get(entity_system::entity from, entity_system::entity to) const;

        entity_system::entity from(entity_system::entity arc) const {
		return m_from[m_system.lookup(arc)];
	}
        entity_system::entity to(entity_system::entity arc) const {
		return m_to[m_system.lookup(arc)];
	}

	std::size_t size() const {
		return m_system.size();
	}

        const entity_system::entity_system & system() const {
		return m_system;
	}

        const std::vector<entity_system::entity> & pin_timing_arcs(entity_system::entity pin) const {
		return m_pin_timing_arcs[m_std_cells.pin_system().lookup(pin)];
	}

        void register_property(entity_system::property* property);

};

} /* namespace timing */
} /* namespace ophidian */

#endif /* SRC_TIMING_LIBRARY_TIMING_ARCS_H_ */
