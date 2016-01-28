/*
 * debug_name_property.h
 *
 *  Created on: Jan 27, 2016
 *      Author: csguth
 */

#ifndef SRC_ENTITY_DEBUG_NAME_PROPERTY_H_
#define SRC_ENTITY_DEBUG_NAME_PROPERTY_H_

#include "property.h"
#include "entity.h"

#include <vector>

namespace openeda {
namespace entity {

class debug_name_property: public property {

	std::vector<std::string> m_names;
	std::vector<entity> m_index2entity;
	std::unordered_map<entity, std::size_t> m_entity2index;

public:
	debug_name_property(system * s);
	virtual ~debug_name_property();

	void destroy( entity e );
	void create( entity e );

	std::vector<std::string>::const_iterator begin() const { return m_names.begin(); }
	std::vector<std::string>::const_iterator end() const { return m_names.end(); }
	std::size_t size() const { return m_names.size(); }
	bool empty() const { return m_names.empty(); }


	void set(entity e, std::string name);
	std::string get(entity e) const;
};

} /* namespace entity */
} /* namespace openeda */

#endif /* SRC_ENTITY_DEBUG_NAME_PROPERTY_H_ */
