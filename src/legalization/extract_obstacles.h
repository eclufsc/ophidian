#ifndef EXTRACT_OBSTACLES_H
#define EXTRACT_OBSTACLES_H

#include "../placement/placement.h"

namespace ophidian {
namespace legalization {
void extract_obstacles(const placement::placement & placement, std::vector<geometry::multi_polygon<geometry::polygon<geometry::point<double>>>> & obstacles);
}
}

#endif // EXTRACT_OBSTACLES_H
