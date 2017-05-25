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

#ifndef OPHIDIAN_ENTITY_SYSTEM_ENTITYSYSTEM_H
#define OPHIDIAN_ENTITY_SYSTEM_ENTITYSYSTEM_H

#include <lemon/bits/alteration_notifier.h>
#include <lemon/maps.h>
#include <lemon/bits/vector_map.h>
#include <lemon/list_graph.h>
#include <iostream>
#include <vector>
#include <deque>

namespace ophidian
{
namespace entity_system
{
class EntitySystemBase;
class EntityBase
{
public:
	friend class EntitySystemBase;
	explicit EntityBase(uint32_t id, EntitySystemBase* system);
	EntityBase();
	~EntityBase();
	bool operator==(const EntityBase& entity) const;
	bool operator!=(const EntityBase& entity) const;
private:
	uint32_t id_;
	EntitySystemBase* system_;
};
class EntitySystemBase
{
public:
	uint32_t id(const EntityBase& en) const;
};


/*! Entity System Notifier */
template <class EntitySystem_, class Entity_>
class EntitySystemNotifier : public lemon::AlterationNotifier<EntitySystem_, Entity_>
{
protected:
	using EntitySystem = EntitySystem_;
	using Entity = Entity_;
	using Parent = lemon::AlterationNotifier<EntitySystem, Entity>;
public:
	class ObserverBase : public Parent::ObserverBase
	{
protected:
public:
		using Parent::ObserverBase::ObserverBase;
		virtual void erase(const std::vector<Entity> &items) final {
		}
		virtual void build() final {
		}
		virtual void reserve(uint32_t size) = 0;
		virtual void shrinkToFit() = 0;
	};
	using Parent::Parent;
	void reserve(uint32_t size)
	{
		for (auto it = Parent::_observers.begin(); it != Parent::_observers.end(); ++it)
		{
			auto observer = static_cast<ObserverBase*>(*it);
			observer->reserve(size);
		}
	}
	void shrinkToFit()
	{
		for (auto it = Parent::_observers.begin(); it != Parent::_observers.end(); ++it)
		{
			auto observer = static_cast<ObserverBase*>(*it);
			observer->shrinkToFit();
		}
	}
	void erase(const Entity & item)
	{
		Parent::erase(item);
	}
private:
	void build()
	{

	}
	void erase(const std::vector<Entity> &items)
	{

	}
};


//! An Entity System
/*!
   Creates, holds and destroys entities in O(1) complexity.
   Does not guarantee any kind of order between the entities.
 */
template <class Entity_>
class EntitySystem final : public EntitySystemBase
{
public:
	using Entity = Entity_;
	using NotifierType = EntitySystemNotifier<EntitySystem, Entity>;
	using ContainerType = std::vector<Entity>;
	using const_iterator = typename ContainerType::const_iterator;
	using size_type = typename ContainerType::size_type;

	//! Construct EntitySystem
	/*!
	   Constructs an empty EntitySystem, with no Entities.
	 */
	EntitySystem()
	{
		notifier_.setContainer(*this);
		id_ = idCounter_++;
	}
	~EntitySystem()
	{

	}

	//! Add Entity
	/*!
	   \brief Creates an Entity instance.
	   \return A handler for the created Entity.
	 */
	Entity add()
	{
		uint32_t id = id2Index_.size();
		Entity entity(id, this);
		id2Index_.push_back(container_.size());
		container_.push_back(entity);
		notifier_.add(container_.back());
		return entity;
	}
	//! Erase Entity
	/*!
	   \param entity A handler for the Entity to erase.
	   \brief Erases an Entity instance as well as its atached Properties.
	 */
	void erase(const Entity& entity)
	{
		notifier_.erase(entity);
		auto entityId = EntitySystemBase::id(entity);
		auto index = id(entity);
		auto lastEntityId = EntitySystemBase::id(container_.back());
		std::swap(container_[index], container_.back());
		container_.pop_back();
		id2Index_[lastEntityId] = index;
		id2Index_[entityId] = std::numeric_limits<uint32_t>::max();
	}
	//! Clear Entities
	/*!
	   \brief Erases all Entities as well as its atached Properties.
	 */
	void clear()
	{
		notifier_.clear();
		container_.clear();
	}
	//! Allocate space for storing Entities
	/*!
	   \brief Using this function, it is possible to avoid superfluous memory allocation: if you know that the EntitySystem you want to build will be large (e.g. it will contain millions entities), then it is worth reserving space for this amount before starting to build the EntitySystem.
	   \param size Minimum capacity for the Entity container.
	 */
	void reserve(size_type size)
	{
		container_.reserve(size);
		notifier_.reserve(size);
	}
	//! Allocate space for storing Entities
	/*!
	   \brief Returns the capacity of the Entity Container. Entities can be added while size < capacity without internal reallocation.
	   \param size Minimum capacity for the Entity container.
	 */
	size_type capacity() const
	{
		return container_.capacity();
	}
	//! Size of the EntitySystem
	/*!
	   \brief Returns the number of Entities
	   \return The number of Entities.
	 */
	size_type size() const
	{
		return container_.size();
	}
	//! Empty EntitySystem
	/*!
	   \brief Returns a boolean indicating if the EntitySystem is empty (i.e., has no entities) or not.
	   \return true if the EntitySystem is empty, false otherwise.
	 */
	bool empty() const
	{
		return container_.empty();
	}
	//! Iterator to beginning
	/*!
	   \brief Returns an iterator pointing to the first element in the EntitySystem.
	   \return Iterator to the first element in the EntitySystem.
	 */
	const_iterator begin() const
	{
		return container_.begin();
	}
	//! Iterator to end
	/*!
	   \brief Returns an iterator referring to the past-the-end element in the EntitySystem.
	   \return Iterator referring to the past-the-end element in the EntitySystem.
	 */
	const_iterator end() const
	{
		return container_.end();
	}
	//! Valid Entity
	/*!
	   \brief Returns a boolean indicating if an Entity handler is pointing to a live entity.
	   \param entity A handler to the Entity we want to check.
	   \return true if the handler is valid, false otherwise.
	 */
	bool valid(const Entity& entity) const
	{
		return EntitySystemBase::id(entity) != std::numeric_limits<uint32_t>::max() &&
		       id2Index_[EntitySystemBase::id(entity)] < container_.size();
	}
	//! Get the Notifier
	/*!
	   \brief Returns a pointer to the System's EntitySystemNotifier.
	   \return A pointer to the System's EntitySystemNotifier.
	 */
	NotifierType* notifier() const
	{
		return const_cast<EntitySystem::NotifierType*>(&notifier_);
	}
	//! Entity id
	/*!
	   \brief Returns the id of an Entity.
	   \param entity A handler to the Entity we want to get the id.
	   \return The id of \p entity.
	 */
	size_type id(const Entity& entity) const
	{
		return id2Index_.at(EntitySystemBase::id(entity));
	}
	//! EntitySystem id
	/*!
	   \return The unique identifier of the EntitySystem.
	 */
	uint32_t id() const {
		return id_;
	}
	//! Shrink EntitySystem
	/*!
	   \brief Reallocate the EntitySystem and it's propertys to have capacity == size. This may help to
	 */
	void shrinkToFit() {
		container_.shrink_to_fit();
		notifier_.shrinkToFit();
	}
private:
	NotifierType notifier_;
	ContainerType container_;
	std::vector<size_type> id2Index_;
	uint32_t id_;
	static uint32_t idCounter_;
};

template <class T>
uint32_t EntitySystem<T>::idCounter_ = 0;


} // namespace entity_system
} // namespace ophidian

#endif // OPHIDIAN_ENTITY_SYSTEM_ENTITYSYSTEM_H
