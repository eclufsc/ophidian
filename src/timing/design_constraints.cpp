/*
 * design_constraints.cpp
 *
 *  Created on: Mar 28, 2016
 *      Author: csguth
 */

#include "design_constraints.h"

namespace openeda {
namespace timing {

default_design_constraints::default_design_constraints()
{
    m_dc.clock.name = "mclk";
    m_dc.clock.port_name = "iccad_clk";
    m_dc.clock.period = 50.0;

}

} /* namespace timing */
} /* namespace openeda */
