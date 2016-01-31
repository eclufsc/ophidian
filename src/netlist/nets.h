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

namespace openeda {
namespace netlist {

class nets: entity::property {
	std::vector<std::vector<entity::entity>> m_pins;
	std::vector<std::string> m_names;

	std::vector<entity::entity> m_index2entity;
	std::unordered_map<entity::entity, std::size_t> m_entity2index;

public:
	nets(entity::system * entity_system);
	virtual ~nets();

	void create(entity::entity e);
	void destroy(entity::entity e);

	std::string name(entity::entity net) const {
		return m_names[m_entity2index.at(net)];
	}
	std::vector<entity::entity> pins(entity::entity net) const {
		return m_pins[m_entity2index.at(net)];
	}
	std::pair< std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator > names() const {
		return std::make_pair(m_names.begin(), m_names.end());
	}

	void connect(entity::entity net, entity::entity pin);
	void disconnect(entity::entity net, entity::entity pin);
	void pins(entity::entity net, std::vector<entity::entity> pins);
	void name(entity::entity net, std::string name);
};

} /* namespace netlist */
} /* namespace openeda */

#endif /* SRC_NETLIST_NETS_H_ */
