/*
 * nets.h
 *
 *  Created on: Jan 28, 2016
 *      Author: csguth
 */

#ifndef SRC_NETLIST_NETS_H_
#define SRC_NETLIST_NETS_H_

#include <vector>
#include <entity.h>
#include <utility>
#include <vector_property.h>

namespace openeda {
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
} /* namespace openeda */

#endif /* SRC_NETLIST_NETS_H_ */
