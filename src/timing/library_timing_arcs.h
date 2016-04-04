/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#ifndef SRC_TIMING_LIBRARY_TIMING_ARCS_H_
#define SRC_TIMING_LIBRARY_TIMING_ARCS_H_

#include "../entity/entity.h"
#include "../standard_cell/standard_cells.h"
#include "lookup_table.h"
#include <unordered_map>

namespace ophidian {
namespace timing {

// Only for pairs of std::hash-able types for simplicity.
// You can of course template this struct to allow other hash functions
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // Mainly for demonstration purposes, i.e. works but is overly simple
        // In the real world, use sth. like boost.hash_combine
        return h1 ^ h2;
    }
};

class library_timing_arcs {
public:
private:
	standard_cell::standard_cells & m_std_cells;

	entity::system m_system;
	entity::vector_property<entity::entity> m_from;
	entity::vector_property<entity::entity> m_to;


	//	// std_cell properties
		entity::vector_property< std::vector<entity::entity> > m_pin_timing_arcs;


		std::unordered_map< std::pair< entity::entity, entity::entity >, entity::entity,pair_hash > m_pinpair2arc;
public:
	library_timing_arcs(standard_cell::standard_cells * std_cells);
	virtual ~library_timing_arcs();
	entity::entity create(entity::entity from, entity::entity to);
	entity::entity get(entity::entity from, entity::entity to) const;

	entity::entity from(entity::entity arc) const {
		return m_from[m_system.lookup(arc)];
	}
	entity::entity to(entity::entity arc) const {
		return m_to[m_system.lookup(arc)];
	}

	std::size_t size() const {
		return m_system.size();
	}

	const entity::system & system() const {
		return m_system;
	}

	const std::vector<entity::entity> & pin_timing_arcs(entity::entity pin) const {
		return m_pin_timing_arcs[m_std_cells.pin_system().lookup(pin)];
	}

	void register_property(entity::property* property);

};

} /* namespace timing */
} /* namespace ophidian */

#endif /* SRC_TIMING_LIBRARY_TIMING_ARCS_H_ */
