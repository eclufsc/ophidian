/*
 * pins.h
 *
 *  Created on: Mar 7, 2016
 *      Author: csguth
 */

#ifndef SRC_PLACEMENT_PINS_H_
#define SRC_PLACEMENT_PINS_H_

#include <vector_property.h>
#include "../netlist/netlist.h"
#include "../geometry/geometry.h"

namespace openeda {
namespace placement {

class pins {
	using point = geometry::point<double>;
	const entity::system & m_system;
	entity::vector_property<point> m_positions;
public:
	pins(openeda::netlist::netlist * netlist);
	virtual ~pins();
	void position(entity::entity pin, point position_point);
	point position(entity::entity pin) const {
		return m_positions[m_system.lookup(pin)];
	}
};

} /* namespace placement */
} /* namespace openeda */

#endif /* SRC_PLACEMENT_PINS_H_ */
