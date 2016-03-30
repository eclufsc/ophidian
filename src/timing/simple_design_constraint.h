/*
 * simple_design_constraint.h
 *
 *  Created on: Mar 28, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_SIMPLE_DESIGN_CONSTRAINT_H_
#define SRC_TIMING_SIMPLE_DESIGN_CONSTRAINT_H_

#include "design_constraints.h"

namespace openeda {
namespace timing {

class simple_design_constraint {
	design_constraints m_dc;
public:
	simple_design_constraint();
	virtual ~simple_design_constraint();
	const design_constraints & dc() const {
		return m_dc;
	}
};

} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_SIMPLE_DESIGN_CONSTRAINT_H_ */
