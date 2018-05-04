#ifndef MULTIBOX_H
#define MULTIBOX_H

#include <ophidian/geometry/Operations.h>

namespace ophidian {
namespace geometry {
//!Class multibox using geometry::Box
template <typename T>
class MultiBox
{
public:

    //!Standar Constructors
    MultiBox() = default;
    MultiBox(const MultiBox<T> &) = default;
    MultiBox(MultiBox<T> &&) = default;
    MultiBox<T>& operator= ( const MultiBox<T> & ) = default;
    MultiBox<T>& operator= ( MultiBox<T> && ) = default;

    //!Constructor receiving a vector of geometry::Box
    MultiBox(const std::vector<geometry::Box<T>> & boxes):
            mBoxes(boxes)
    {
    }

    //!Push back a geometry::Box
    void push_back(const Box<T> & box)
    {
        mBoxes.push_back(box);
    }

    //!Non-const iterator begin
    typename
    std::vector<Box<T>>::iterator begin()
    {
        return mBoxes.begin();
    }

    //!Non-const iterator end
    typename
    std::vector<Box<T>>::iterator end()
    {
        return mBoxes.end();
    }

    //!Const iterator begin
    typename
    std::vector<Box<T>>::const_iterator begin() const
    {
        return mBoxes.begin();
    }

    //!Const iterator end
    typename
    std::vector<Box<T>>::const_iterator end() const
    {
        return mBoxes.end();
    }

    //!Operator overloading for comparison of two multibox objects
    bool operator==(const MultiBox & other) const
    {
        for(auto box1 : this->mBoxes)
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

    //!Operator overload for difference between two multibox objects
    bool operator!=(const MultiBox<T> & other) const
    {
        return !(*this == other);
    }

    MultiBox<T> translate(Point<T> translationPoint)
    {
        std::vector<Box<T>> translatedBoxes;
        translatedBoxes.reserve(mBoxes.size());
//        for(auto box : mBoxes)
//        {
//            Box<T> translatedBox = geometry::translate(box, translationPoint);
//            translatedBoxes.push_back(translatedBox);
//        }

        return MultiBox{translatedBoxes};
    }

private:
    std::vector<Box<T>> mBoxes;
};
}
}

#endif // MULTIBOX_H
