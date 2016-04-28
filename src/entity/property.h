/*
 * Copyright 2016 Ophidian
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
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
