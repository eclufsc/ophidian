/*
 * Copyright 2017 Ophidian
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

#ifndef OPHIDIAN_ENTITY_SYSTEM_ENTITYSYSTEMMAP_H
#define OPHIDIAN_ENTITY_SYSTEM_ENTITYSYSTEMMAP_H

#include <lemon/maps.h>
#include "EntitySystem.h"

namespace ophidian
{
namespace entity_system
{
template <class Entity_, class Value_>
class Property :
	public lemon::MapBase<Entity_, Value_>,
	public EntitySystem<Entity_>::NotifierType::ObserverBase
{
public:
	using Parent = typename EntitySystem<Entity_>::NotifierType::ObserverBase;
	using Value = Value_;
	using Entity = Entity_;
	using ContainerType = std::vector<Value>;

	Property(const EntitySystem<Entity_>& system, Value defaultValue = Value()) :
		Parent(*system.notifier()),
		defaultValue_(defaultValue)
	{
		properties_.reserve(system.capacity());
		properties_.resize(system.size());
		properties_.assign(properties_.size(), defaultValue_);
	}

	Property() :
		Parent()
	{

	}

	Property& operator=(const Property& o)
	{
		properties_ = o.properties_;
		Parent::attach(*o.notifier());
		return *this;
	}

	~Property() override
	{

	}

	typename ContainerType::reference operator[](const Entity& entity)
	{
		return properties_[Parent::notifier()->id(entity)];
	}
	typename ContainerType::const_reference operator[](const Entity& entity) const
	{
		return properties_[Parent::notifier()->id(entity)];
	}

	typename ContainerType::iterator begin()
	{
		return properties_.begin();
	}
	typename ContainerType::iterator end()
	{
		return properties_.end();
	}

	typename ContainerType::const_iterator begin() const
	{
		return properties_.begin();
	}
	typename ContainerType::const_iterator end() const
	{
		return properties_.end();
	}
	typename ContainerType::size_type size() const
	{
		return properties_.size();
	}
	bool empty() const
	{
		return properties_.empty();
	}

	void reserve(std::uint32_t size) override
	{
		properties_.reserve(size);
	}

	void shrinkToFit() override
	{
		properties_.shrink_to_fit();
	}

	std::uint32_t capacity() const
	{
		return properties_.capacity();
	}

protected:

	virtual void add(const Entity& item) override
	{
		properties_.push_back(defaultValue_);
	}
	virtual void add(const std::vector<Entity>& items) override
	{
		properties_.resize(properties_.size() + items.size(), defaultValue_);
	}
	virtual void erase(const Entity& item) override
	{
		std::swap(properties_.back(), properties_[Parent::notifier()->id(item)]);
		properties_.pop_back();
	}

	virtual void clear() override
	{
		properties_.clear();
	}

protected:
	ContainerType properties_;
private:
	const Value defaultValue_;
};
} // namespace entity_system
} // namespace ophidian

#endif // OPHIDIAN_ENTITY_SYSTEM_ENTITYSYSTEMMAP_H
