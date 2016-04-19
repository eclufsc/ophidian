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

#ifndef ophidian_VECTOR_PROPERTY_H
#define ophidian_VECTOR_PROPERTY_H

#include <vector>

#include "entity.h"

namespace ophidian {
    namespace entity {
        /// Implementation of the property class.
        /**
         * Describes an implementation of the property class that stores the properties in a vector, acessed by the entity index.
         * \tparam T Type of the property value to be stored.
         */
        template <class T>
        class vector_property : public property {
            std::vector<T> m_values;

        public:
            /// Constructor.
            /**
             * Default constructor. Creates an empty vector property.
             */
            vector_property(){

            }

            /// Destroys the property of an entity.
            /**
             * Implements the destroy method from property. Moves the last property in the vector to the position of the destroyed entity.
             * \param e Entity that was destroyed
	         * \param index The index of the destroyed entity
             */
            void destroy(entity & e, std::size_t index) {
                std::size_t last_index = m_values.size() - 1;
                m_values[index] = m_values[last_index];
                m_values.pop_back();
            }
            /// Creates property for an entity.
            /**
             * Implements the create method from property. Resizes the vector to contain the property of the new entity.
             * \param e Entity that was created
             * \param index The index of the created entity
             */
            void create(entity & e, std::size_t index) {
                if (index >= m_values.size()) {
                    m_values.resize(index + 1);
                }

            }

            /// Property getter
            /**
             * Returns the property value for an entity.
             * \param entity_index The index of the entity to get the property.
             * \return A reference to the property of that entity.
             */
            T &operator[](std::size_t entity_index) {
                return m_values[entity_index];
            }

            /// Constant property getter
            /**
             * Returns the property value for an entity. Necessary when there is only a constant reference to the properties.
             * \param entity_index The index of the entity to get the property.
             * \return A constant reference to the property of that entity.
             */
            const T &operator[](std::size_t entity_index) const {
                return m_values[entity_index];
            }

            /// Begin iterator
            /**
             * Returns an iterator pointing to the beginning of the vector of properties.
             * \return Iterator pointing to the beginning of the vector of properties.
             */
            typename std::vector<T>::const_iterator begin() const {
                return m_values.begin();
            }

            /// End iterator
            /**
             * Returns an iterator pointing to the end of the vector of properties.
             * \return Iterator pointing to the end of the vector of properties.
             */
            typename std::vector<T>::const_iterator end() const {
                return m_values.end();
            }
        };

        template <>
        class vector_property<bool> : public property {
            std::vector<bool> m_values;

        public:
            void destroy(entity & e, std::size_t index) {
                std::size_t last_index = m_values.size() - 1;
                m_values[index] = m_values[last_index];
                m_values.pop_back();
            }
            void create(entity & e, std::size_t index) {
                if (index >= m_values.size()) {
                    m_values.resize(index + 1);
                }

            }

            std::vector<bool>::reference operator[](std::size_t entity_index) {
                return m_values[entity_index];
            }

            bool operator[](std::size_t entity_index) const {
                return m_values[entity_index];
            }

            std::vector<bool>::const_iterator begin() const {
                return m_values.begin();
            }

            std::vector<bool>::const_iterator end() const {
                return m_values.end();
            }
        };
    }
}


#endif //ophidian_VECTOR_PROPERTY_H
