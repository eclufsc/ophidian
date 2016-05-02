#ifndef OPHIDIAN_FLOORPLAN_LEFDEF2FLOORPLAN_H
#define OPHIDIAN_FLOORPLAN_LEFDEF2FLOORPLAN_H

#include "../parsing/lef.h"
#include "../parsing/def.h"
#include "floorplan.h"

namespace ophidian {
namespace floorplan {

void lefdef2floorplan(const parsing::lef & lef, const parsing::def & def, floorplan & fplan);

}
}

#endif // OPHIDIAN_FLOORPLAN_LEFDEF2FLOORPLAN_H
