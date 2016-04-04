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

#ifndef OPENEDA_VECTOR_PROPERTY_H
#define OPENEDA_VECTOR_PROPERTY_H

#include <vector>

#include "entity.h"

namespace ophidian {
    namespace entity {
        template <class T>
        class vector_property : public property {
            std::vector<T> m_values;

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
    }
}


#endif //OPENEDA_VECTOR_PROPERTY_H
