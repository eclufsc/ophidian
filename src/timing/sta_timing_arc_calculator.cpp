/*
 * sta_timing_arc_calculator.cpp
 *
 *  Created on: Mar 22, 2016
 *      Author: csguth
 */

#include "sta_timing_arc_calculator.h"

namespace openeda {
namespace timing {

sta_timing_arc_calculator::sta_timing_arc_calculator(const library & lib) :
		 m_library(lib) {
}

sta_timing_arc_calculator::~sta_timing_arc_calculator() {
}

} /* namespace timing */
} /* namespace openeda */
