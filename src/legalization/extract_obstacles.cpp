#include "extract_obstacles.h"

namespace ophidian {
namespace legalization {

void extract_obstacles(const placement::placement &placement, std::vector<geometry::multi_polygon<geometry::polygon<geometry::point<double> > > > &obstacles)
{
    obstacles.reserve(placement.netlist().cell_count());
    for (auto cell : placement.netlist().cell_system()) {
        if (placement.cell_fixed(cell)) {
            obstacles.push_back(placement.cell_geometry(cell));
        }
    }
    obstacles.shrink_to_fit();
}

}
}

