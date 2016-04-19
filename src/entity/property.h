/*
 * property.h
 *
 *  Created on: Jan 27, 2016
 *      Author: csguth
 */

#ifndef SRC_ENTITY_PROPERTY_H_
#define SRC_ENTITY_PROPERTY_H_

#include <cstdlib>

namespace ophidian {
namespace entity {

class entity;
/// Property class.
/**
 * Describes a property that can be associated to an entity system.
 */
class property {
public:
	/// Virtual destructor.
	virtual ~property() { };
	/// Virtual destroy method.
	/**
	 * This method is called when an entity in the system is destroyed.
	 * \param e Entity that was destroyed
	 * \param index The index of the destroyed entity
	 */
	virtual void destroy( entity & e, std::size_t index) = 0;
	/// Virtual create method.
	/**
	 * This method is called when a new entity is created in the system.
	 * \param e Entity that was created
	 * \param index The index of the created entity
	 */
	virtual void create( entity & e, std::size_t index) = 0;
};

} /* namespace entity */
} /* namespace openeda */

#endif /* SRC_ENTITY_PROPERTY_H_ */
