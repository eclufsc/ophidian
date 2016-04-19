//
// Created by renan on 2/1/16.
//

#ifndef OPENEDA_VECTOR_PROPERTY_H
#define OPENEDA_VECTOR_PROPERTY_H

#include <vector>

#include "entity.h"

namespace openeda {
    namespace entity {
        template <class T>
        class vector_property : public property {
            std::vector<T> m_values;

        public:
            vector_property() {

            }

            vector_property(T default_value) : m_values(default_value) {

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

            T &operator[](std::size_t entity_index) {
                return m_values[entity_index];
            }

            const T &operator[](std::size_t entity_index) const {
                return m_values[entity_index];
            }

            typename std::vector<T>::const_iterator begin() const {
                return m_values.begin();
            }

            typename std::vector<T>::const_iterator end() const {
                return m_values.end();
            }
        };

        template <>
        class vector_property<bool> : public property {
            std::vector<bool> m_values;

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


#endif //OPENEDA_VECTOR_PROPERTY_H
