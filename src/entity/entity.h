/*
 * entity.h
 *
 *  Created on: Jan 27, 2016
 *      Author: csguth
 */

#ifndef SRC_ENTITY_ENTITY_H_
#define SRC_ENTITY_ENTITY_H_

#include <unordered_map>
#include <limits>
#include <set>

#include "property.h"

namespace openeda {
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
    template<> struct hash<openeda::entity::entity>
    {
        typedef openeda::entity::entity argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& s) const
        {
            result_type const h1 ( std::hash<std::uint32_t>()(s.id()) );
            return h1;
        }
    };
}


namespace openeda {
namespace entity{

class system {
	std::unordered_map<uint32_t, entity> m_entities;
	uint32_t m_next;

	std::set<property*> m_properties;

public:
	system();
	entity create();
	void destroy( entity e );
	std::size_t size() const { return m_entities.size(); }
	bool empty() const { return m_entities.empty(); }
	std::unordered_map<uint32_t, entity>::const_iterator begin() const { return m_entities.begin(); }
	std::unordered_map<uint32_t, entity>::const_iterator end() const { return m_entities.end(); }

	void register_property( property * p );

};

} /* namespace entity */
} /* namespace openeda */

#endif /* SRC_ENTITY_ENTITY_H_ */
