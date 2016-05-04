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

            void _m_range_check(size_t index) const {
                if(index >= m_values.size())
                    throw std::out_of_range("vector::_M_range_check");
            }

        public:
            /// Constructor.
            /**
             * Default constructor. Creates an empty vector property.
             */
            vector_property() {

            }

            vector_property(T default_value) : m_values(default_value) {

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

            /// Property getter with range check
            /**
             * Returns the property value for an entity after checking the range.
             * \param entity_index The index of the entity to get the property.
             * \return A reference to the property of that entity.
             */
            T & at(std::size_t entity_index) {
                _m_range_check(entity_index);
                return m_values[entity_index];
            }

            /// Constant property getter with range check
            /**
             * Returns the property value for an entity after checking the range. Necessary when there is only a constant reference to the properties.
             * \param entity_index The index of the entity to get the property.
             * \return A constant reference to the property of that entity.
             */
            const T & at(std::size_t entity_index) const {
                _m_range_check(entity_index);
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

            void preallocate(std::size_t qnt) {
                m_values.reserve(std::max(m_values.capacity(), qnt));
            }
        };

        template <>
        class vector_property<bool> : public property {
            std::vector<bool> m_values;

            void _m_range_check(size_t index) const {
                if(index >= m_values.size())
                    throw std::out_of_range("vector::_M_range_check");
            }

        public:
            vector_property() {

            }

            vector_property(bool default_value) : m_values(default_value) {

            }

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

            bool operator[](std::size_t entity_index) const {
                return m_values[entity_index];
            }

            std::vector<bool>::reference operator[](std::size_t entity_index) {
                return m_values[entity_index];
            }

            bool at(std::size_t entity_index) const {
                _m_range_check(entity_index);
                return m_values[entity_index];
            }

            std::vector<bool>::reference at(std::size_t entity_index) {
                _m_range_check(entity_index);
                return m_values[entity_index];
            }

            std::vector<bool>::const_iterator begin() const {
                return m_values.begin();
            }

            std::vector<bool>::const_iterator end() const {
                return m_values.end();
            }

            void preallocate(std::size_t qnt) {
                m_values.reserve(std::max(m_values.capacity(), qnt));
            }
        };
    }
}


#endif //ophidian_VECTOR_PROPERTY_H
