/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#ifndef SRC_ENTITY_ENTITY_H_
#define SRC_ENTITY_ENTITY_H_

#include <unordered_map>
#include <limits>
#include <set>
#include <vector>

#include <boost/bimap.hpp>

#include "property.h"

/// The namespace of ophidian
namespace ophidian {
/// Namespace describing entities, systems and properties
namespace entity {
/// Entity class
		/**
		 * This class describes an entity from an entity system, which can have properties associated to it.
		 */
class entity {
    uint32_t m_id;
public:

	/// Constructor.
	/**  Entity constructor.
	 *  \param id Entity identifier.
	 */
	entity(uint32_t id = std::numeric_limits<uint32_t>::max());

	/// Identity getter.
	/**
	 * Getter for the entity identifier.
	 * \return Identifier of the entity
	 */
	uint32_t id() const {
		return m_id;
	}

	/// Less than operator.
	/**
	 * Implements the less than operator.
	 * \param o Entity to be compared with.
	 * \return true if entity identifier is less than identifier of entity o, false otherwise.
	 */
	bool operator<(entity o) const {
		return m_id < o.m_id;
	}

	/// Equality operator.
	/**
	 * Implements the equality operator.
	 * \param o Entity to be compared with.
	 * \return true if entity identifier is equal to identifier of entity o, false otherwise.
	 */
	bool operator==(entity o) const {
		return m_id == o.m_id;
	}
};

}
}

// custom specialization of std::hash can be injected in namespace std
namespace std
{
template<> struct hash<ophidian::entity::entity>
{
    typedef ophidian::entity::entity argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& s) const
    {
        result_type const h1 ( std::hash<std::uint32_t>()(s.id()) );
        return h1;
    }
};
}


namespace ophidian {
namespace entity{

	using entity2index_map = typename boost::bimap< entity, std::size_t >;
	using entity2index_map_iterator = typename entity2index_map::left_const_iterator;

	class bimap_iterator_adapter {
		entity2index_map_iterator m_it;
	public:
		bimap_iterator_adapter(entity2index_map_iterator it) :
				m_it(it){

		}
		bimap_iterator_adapter(const bimap_iterator_adapter & o) :
				m_it(o.m_it){

		}
		bimap_iterator_adapter& operator=(const bimap_iterator_adapter & o) {
			m_it = o.m_it;
			return *this;
		}
		virtual ~bimap_iterator_adapter()
		{
		}
		bimap_iterator_adapter operator++(int) {
			m_it++;
		}
		bimap_iterator_adapter operator--(int) {
			m_it--;
		}
		bimap_iterator_adapter& operator++() {
			++m_it;
		}
		bimap_iterator_adapter& operator--() {
			--m_it;
		}
		bool operator!=(const bimap_iterator_adapter & other) const {
			return m_it != other.m_it;
		}
		entity operator*() {
			return m_it->first;
		}
	};

	/// System class.
	/*
	 * This class describes an entity system, which stores all its entities and pointers to the properties associated to them.
	 */
class system {
	using entity2index_map = typename boost::bimap< entity, std::size_t >;
	entity2index_map m_entities;
	uint32_t m_next;

    std::set<property*> m_properties;
public:
	/// Constructor.
	/**
	 * Creates an empty system.
	 */
	system();
	/// Creates an entity.
	/**
	 * Creates a new entity and add it to the system.
	 * If there are properties associtated to the system they are notified that a new entity has been created.
	 * \return The created entity.
	 */
	entity create();
	/// Destroy an entity.
	/**
	 * Destroys a entity from the system.
	 * If there are properties associtated to the system they are notified that an entity has been destroyed.
	 * \param e Entity to be destroyed
	 */
	void destroy( entity e );
	/// Returns the size of the system.
	/**
	 * Returns the number of entities as the size of the system.
	 * \return Number of entities in the system.
	 */
	std::size_t size() const { return m_entities.size(); }
	/// Returns if the system is empty.
	/**
	 * Returns if the system is empty or not. An empty system do not have entities belonging to it.
	 * \return true if the system is empty, false otherwise.
	 */
	bool empty() const { return m_entities.empty(); }
	/// Begin iterator.
	/**
	 * Returns an iterator pointing to the beginning of the entities in the system.
	 * The iterator can be used to iterate through the entities.
	 * \return Iterator pointing to the beginning of the entities in the system.
	 */
	entity2index_map::left_const_iterator begin() const { return m_entities.left.begin(); }
	/// End iterator.
	/**
	 * Returns an interator pointing to the end of the entities in the system.
	 * \return Iterator pointing to the end of the entities in the system.
	 */
	entity2index_map::left_const_iterator end() const { return m_entities.left.end(); }

	/// Registers property.
	/**
	 *	Registers a new property in the system. If tere are already entities created in the system the property is notified to create values to them.
	 *	\param p Pointer to the new property.
	 */
	void register_property( property * p );

	/// Entities map getter.
	/**
	 * Getter of the entities map.
	 * \return A constant reference to the map containing all entities in the system.
	 */
	const entity2index_map::left_map & entities() const { return m_entities.left; }

	/// Gets the index of an entity
	/**
	 * Gets the index of an entity, which can be used to acess its properties.
	 * \param e Entity to lookup.
	 * \return Index of the entity.
	 */
	std::size_t lookup(entity & e) const { return m_entities.left.at(e); }

};


} /* namespace entity */
} /* namespace ophidian */

#endif /* SRC_ENTITY_ENTITY_H_ */
