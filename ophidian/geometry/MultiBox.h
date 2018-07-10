#ifndef MULTIBOX_H
#define MULTIBOX_H

#include <vector>

#include "Models.h"

namespace ophidian {
namespace geometry {

    template <typename T>
    class MultiBox
    {
    public:
        // Member types
        using value_type = Box<T>;

        // Constructors
        MultiBox() = default;

        MultiBox(const MultiBox<T> &) = default;
        MultiBox<T>& operator= ( const MultiBox<T> & ) = default;

        MultiBox(MultiBox<T> &&) = default;
        MultiBox<T>& operator= ( MultiBox<T> && ) = default;

        // template <class Vector>
        // MultiBox(Vector&& boxes):
        //     m_boxes{std::forward<Vector>(boxes)}
        // { }

        MultiBox(const std::vector<Box<T>>& boxes):
            m_boxes{boxes}
        {}

        MultiBox(std::vector<Box<T>>&& boxes):
            m_boxes{std::move(boxes)}
        {}

        // Element access

        // Iterators
        typename
        std::vector<Box<T>>::iterator begin()
        {
            return m_boxes.begin();
        }

        typename
        std::vector<Box<T>>::iterator end()
        {
            return m_boxes.end();
        }

        typename
        std::vector<Box<T>>::const_iterator begin() const
        {
            return m_boxes.begin();
        }

        typename
        std::vector<Box<T>>::const_iterator end() const
        {
            return m_boxes.end();
        }

        // Capacity
        typename
        std::vector<Box<T>>::size_type size() const noexcept
        {
            return m_boxes.size();
        }

        // Modifiers
        void push_back(const Box<T> & box)
        {
            m_boxes.push_back(box);
        }

        bool operator==(const MultiBox & other) const noexcept
        {
            for(auto box1 : m_boxes)
            {
                for(auto box2 : other.mBoxes)
                {
                    bool comparison = (box1.min_corner().x() == box2.min_corner().x()) &&
                                      (box1.min_corner().y() == box2.min_corner().y()) &&
                                      (box1.max_corner().x() == box2.max_corner().x()) &&
                                      (box1.max_corner().y() == box2.max_corner().y());
                    if(!comparison) {
                        return false;
                    }
                }
            }

            return true;
        }

        bool operator!=(const MultiBox<T> & other) const noexcept
        {
            return !(*this == other);
        }

    private:
        std::vector<Box<T>> m_boxes;
    };
}
}

#endif // MULTIBOX_H
