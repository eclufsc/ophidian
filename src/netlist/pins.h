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
#include <vector_property.h>

namespace openeda {
namespace netlist {

class pins {
	entity::system & m_system;

	entity::vector_property<entity::entity> m_owners;
	entity::vector_property<entity::entity> m_nets;
	entity::vector_property<entity::entity> m_std_cell_pin;

public:
	pins(entity::system & system);
	virtual ~pins();

	entity::entity owner(entity::entity pin) const {
		return m_owners[m_system.lookup(pin)];
	}
	entity::entity net(entity::entity pin) const {
		return m_nets[m_system.lookup(pin)];
	}
	entity::entity standard_cell_pin(entity::entity pin) const {
		return m_std_cell_pin[m_system.lookup(pin)];
	}

	void owner(entity::entity pin, entity::entity owner);
	void net(entity::entity pin, entity::entity net);
	void standard_cell_pin(entity::entity pin, entity::entity std_cell_pin);

};

} /* namespace netlist */
} /* namespace openeda */

#endif /* SRC_NETLIST_PINS_H_ */
