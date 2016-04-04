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

#ifndef SRC_ENTITY_ENTITY_H_
#define SRC_ENTITY_ENTITY_H_

#include <unordered_map>
#include <limits>
#include <set>
#include <vector>

#include <boost/bimap.hpp>

#include "property.h"

namespace ophidian {
namespace entity {

class entity {
	uint32_t m_id;
public:
	entity(uint32_t id = std::numeric_limits<uint32_t>::max());
	uint32_t id() const {
		return m_id;
	}
	bool operator<(entity o) const {
		return m_id < o.m_id;
	}
	bool operator==(entity o) const {
		return m_id == o.m_id;
	}
};

}
}

// custom specialization of std::hash can be injected in namespace std
namespace std
{
    template<> struct hash<ophidian::entity::entity>
    {
        typedef ophidian::entity::entity argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& s) const
        {
            result_type const h1 ( std::hash<std::uint32_t>()(s.id()) );
            return h1;
        }
    };
}


namespace ophidian {
namespace entity{

class system {
	using entity2index_map = typename boost::bimap< entity, std::size_t >;
	entity2index_map m_entities;
	uint32_t m_next;

	std::set<property*> m_properties;

public:
	system();
	entity create();
	void destroy( entity e );
	std::size_t size() const { return m_entities.size(); }
	bool empty() const { return m_entities.empty(); }
	entity2index_map::left_const_iterator begin() const { return m_entities.left.begin(); }
	entity2index_map::left_const_iterator end() const { return m_entities.left.end(); }

	void register_property( property * p );

	const entity2index_map::left_map & entities() const { return m_entities.left; }

	std::size_t lookup(entity & e) const { return m_entities.left.at(e); }

};

} /* namespace entity */
} /* namespace openeda */

#endif /* SRC_ENTITY_ENTITY_H_ */
