/*
 * This file is part of OpenEDA.
 *
 * OpenEDA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenEDA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenEDA.  If not, see <http://www.gnu.org/licenses/>.
 *
 * recycling_vector.h
 *
 *  Created on: Dec 10, 2015
 *      Author: csguth
 */

#ifndef OPENEDA_RECYCLING_VECTOR_H_
#define OPENEDA_RECYCLING_VECTOR_H_

#include <unordered_set>

namespace openeda {
namespace data_structures {

template<class ElementType, class IdentifierType>
class recycling_vector {
	typename std::vector<ElementType> m_container;
	typename std::unordered_set<IdentifierType> m_recycled;
public:

	class iterator {
		friend class recycling_vector;
		typename std::vector<ElementType> * m_container;
		typename std::unordered_set<IdentifierType> * m_recycled;
		IdentifierType m_i;
		iterator(std::vector<ElementType> * container,
				std::unordered_set<IdentifierType> * recycled, IdentifierType i) :
					m_container(container), m_recycled(recycled), m_i(i) {
		}
	public:
		void operator++(void) {
			do {
				++m_i;
			} while (m_recycled->find(m_i) != m_recycled->end());
			if (m_i >= m_container->size())
				m_i = m_container->size();
		}
		void operator--(void) {
			if(m_i == 0)
				return;
			do {
				--m_i;
			} while (m_i != 0 && m_recycled->find(m_i) != m_recycled->end());
		}
		bool operator!=(iterator i) {
			return i.m_i != m_i;
		}
		IdentifierType operator*() const {
			return m_i;
		}
	};

	recycling_vector() {

	}
	virtual ~recycling_vector() {

	}
	IdentifierType insert(ElementType e) {
		IdentifierType id = m_container.size();
		if (!m_recycled.empty()) {
			id = *m_recycled.begin();
			m_recycled.erase(m_recycled.begin());
		}
		m_container.resize(std::max(id + 1, m_container.size()));
		m_container[id] = e;
		return id;
	}
	void erase(IdentifierType id) {
		if (m_recycled.find(id) != m_recycled.end())
			throw std::runtime_error("Element not found!!");
		m_recycled.insert(id);
	}
	std::size_t size() const {
		return m_container.size() - m_recycled.size();
	}
	std::size_t empty() const {
		return size() == 0;
	}
	ElementType& operator[](IdentifierType i) {
		if (m_recycled.find(i) != m_recycled.end())
			throw std::runtime_error("Element not found!!");
		return m_container.at(i);
	}

	iterator begin() {
		std::size_t first = 0;
		auto lowerbound_it = std::min_element(m_recycled.begin(), m_recycled.end());
		auto it = iterator(&m_container, &m_recycled, 0);
		if(lowerbound_it != m_recycled.end())
			if(*lowerbound_it == 0)
				++it;
		return it;
	}

	iterator end() {
		return iterator(&m_container, &m_recycled, m_container.size());
	}


};

} /* namespace data_structures */
} /* namespace openeda */

#endif /* OPENEDA_RECYCLING_VECTOR_H_ */
