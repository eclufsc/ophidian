#ifndef OPHIDIAN_GEOMETRY_DISTANCE_H
#define OPHIDIAN_GEOMETRY_DISTANCE_H

#include "Models.h"

namespace ophidian {
namespace geometry {

class ManhattanDistance {
public:
	ManhattanDistance() {

	}

    double operator()(const Point & point1, const Point & point2);
};

class EuclideanDistance {
public:
	EuclideanDistance() {

	}

    double operator()(const Point &point1, const Point &point2);
};

}
}

#endif // OPHIDIAN_GEOMETRY_DISTANCE_H
