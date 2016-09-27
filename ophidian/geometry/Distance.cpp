#include "Distance.h"

namespace ophidian {
namespace geometry {

double ManhattanDistance::operator()(const Point &point1, const Point &point2) {
  double distance = std::abs(point1.x() - point2.x()) + std::abs(point1.y() - point2.y());
  return distance;
}

double EuclideanDistance::operator()(const Point & point1, const Point & point2) {
  double distanceX = (point1.x() - point2.x()) * (point1.x() - point2.x());
  double distanceY = (point1.y() - point2.y()) * (point1.y() - point2.y());
  double distance = std::sqrt(distanceX + distanceY);
  return distance;
}

}
}
