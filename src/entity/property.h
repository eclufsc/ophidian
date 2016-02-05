/*
 * property.h
 *
 *  Created on: Jan 27, 2016
 *      Author: csguth
 */

#ifndef SRC_ENTITY_PROPERTY_H_
#define SRC_ENTITY_PROPERTY_H_

#include <cstdlib>

namespace openeda {
namespace entity {

class entity;
class property {
public:
	virtual ~property() { };
	virtual void destroy( entity & e, std::size_t index) = 0;
	virtual void create( entity & e, std::size_t index) = 0;
};

} /* namespace entity */
} /* namespace openeda */

#endif /* SRC_ENTITY_PROPERTY_H_ */
