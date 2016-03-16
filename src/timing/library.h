/*
 * library.h
 *
 *  Created on: Mar 16, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_LIBRARY_H_
#define SRC_TIMING_LIBRARY_H_

#include "../standard_cell/standard_cells.h"
#include "../entity/vector_property.h"

#include <boost/units/systems/si.hpp>
#include <boost/units/io.hpp>

namespace openeda {
namespace timing {

class library {
	openeda::standard_cell::standard_cells & m_std_cells;
	entity::vector_property< boost::units::quantity< boost::units::si::capacitance > > m_pin_capacitance;
public:
	library(openeda::standard_cell::standard_cells * std_cells);
	virtual ~library();



	void pin_capacitance(entity::entity pin, boost::units::quantity< boost::units::si::capacitance > capacitance);
	boost::units::quantity< boost::units::si::capacitance > pin_capacitance(entity::entity pin) const {
		return m_pin_capacitance[m_std_cells.pin_system().lookup(pin)];
	}


	openeda::standard_cell::standard_cells & std_cells()
	{
		return m_std_cells;
	}
};

} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_LIBRARY_H_ */
