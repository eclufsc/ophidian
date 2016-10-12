#ifndef OPHIDIAN_INTERCONNECTION_FLUTE_H
#define OPHIDIAN_INTERCONNECTION_FLUTE_H

#include <ophidian/geometry/Models.h>
#include <vector>
#include <memory>

namespace ophidian {
namespace interconnection {

class SteinerTree;

//! FLUTE: Fast Lookup Table Based Technique for RSMT Construction and Wirelength Estimation
/*!
  From: http://home.eng.iastate.edu/~cnchu/flute.html
 */
class Flute final {
public:
    //! Singleton instantiation
    static Flute& instance();
    ~Flute();
    //! Create Steiner Tree
    /*!
        \brief Creates a Steiner Tree from a container of geometry::Point using FLUTE algorithm.
        \param container A container of geometry::Point.
        \return A std::unique_ptr to the created Steiner Tree.
     */
    template <class T>
    inline std::unique_ptr<SteinerTree> create(const T& container) {
        const uint32_t kSize = container.size();
        if (kSize == 1) {
            return trivialSteinerTree(*std::begin(container));
        } else if (kSize == 2) {
            typename T::const_iterator it(container.begin());
            typename T::const_iterator it2(it);
            ++it2;
            return singleSegment(*it, *it2);
        }
        std::vector<unsigned> X(kSize);
        std::vector<unsigned> Y(kSize);
        X.resize(0);
        Y.resize(0);

        geometry::Point offset(0.0, 0.0);

        for (const auto & point : container) {
            offset.x(std::min(offset.x(), point.x()));
            offset.y(std::min(offset.y(), point.y()));
        }

        offset.x(-offset.x());
        offset.y(-offset.y());

        for (const auto & point : container) {
            X.push_back(static_cast<unsigned>(std::round(point.x() + offset.x())));
            Y.push_back(static_cast<unsigned>(std::round(point.y() + offset.y())));
        }

        return callFlute(X, Y, offset);
    }

private:
    std::unique_ptr<SteinerTree> singleSegment(const geometry::Point& p1, const geometry::Point& p2);
    std::unique_ptr<SteinerTree> trivialSteinerTree(const geometry::Point& p);
    std::unique_ptr<SteinerTree> callFlute(const std::vector<unsigned>& X, const std::vector<unsigned>& Y, const geometry::Point &offset);
    Flute();
    Flute(const Flute& o) = delete;
    Flute& operator=(const Flute& o) = delete;
};

}
}

#endif // OPHIDIAN_INTERCONNECTION_FLUTE_H
