#ifndef OPHIDIAN_GEOMETRY_DISTANCE_H
#define OPHIDIAN_GEOMETRY_DISTANCE_H

#include "Models.h"

namespace ophidian {
namespace geometry {

class ManhattanDistance {
public:
    //! Construct ManhattanDistance
    /*!
     * \brief Empty default constructor
     */
	ManhattanDistance() {

	}

    //! Distance operator
    /*!
     * \brief Calculates the Manhattan distance between two points
     * \param point1 First point to calculate the distance
     * \param point2 Second point to calculate the distance
     * \return Manhattan distance between point1 and point2
     */
    double operator()(const Point & point1, const Point & point2);
};

class EuclideanDistance {
public:
    //! Construct EuclideanDistance
    /*!
     * \brief Empty default constructor
     */
	EuclideanDistance() {

	}

    //! Distance operator
    /*!
     * \brief Calculates the Euclidean distance between two points
     * \param point1 First point to calculate the distance
     * \param point2 Second point to calculate the distance
     * \return Euclidean distance between point1 and point2
     */
    double operator()(const Point &point1, const Point &point2);
};

}
}

#endif // OPHIDIAN_GEOMETRY_DISTANCE_H
