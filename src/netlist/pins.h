/*
 * pins.h
 *
 *  Created on: Jan 28, 2016
 *      Author: csguth
 */

#ifndef SRC_NETLIST_PINS_H_
#define SRC_NETLIST_PINS_H_

#include <vector>
#include <entity.h>
#include <property.h>
#include "cells.h"

namespace openeda {
namespace netlist {

class cells;
class pins: entity::property {
	std::vector<entity::entity> m_owners;
	std::vector<std::string> m_names;

	std::vector<entity::entity> m_index2entity;
	std::unordered_map<entity::entity, std::size_t> m_entity2index;

public:
	pins(entity::system * entity_system);
	virtual ~pins();

	void create(entity::entity e);
	void destroy(entity::entity e);

	entity::entity owner(entity::entity pin) const {
		return m_owners[m_entity2index.at(pin)];
	}
	std::string name(entity::entity pin) const {
		return m_names[m_entity2index.at(pin)];
	}

	void owner(entity::entity pin, entity::entity owner);
	void name(entity::entity pin, std::string name);

};

} /* namespace netlist */
} /* namespace openeda */

#endif /* SRC_NETLIST_PINS_H_ */
