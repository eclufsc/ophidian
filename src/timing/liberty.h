/*
 * liberty.h
 *
 *  Created on: Mar 16, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_LIBERTY_H_
#define SRC_TIMING_LIBERTY_H_

#include "library.h"
#include <string>

namespace openeda {
namespace timing {
namespace liberty {

void read(std::string filename, timing::library& library);

} /* namespace liberty */
} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_LIBERTY_H_ */
