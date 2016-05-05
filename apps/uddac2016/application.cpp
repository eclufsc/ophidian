#include "application.h"

#include "../placement/def2placement.h"
#include "../placement/lef2library.h"

namespace uddac2016 {

using namespace ophidian;


application::application() :
    m_netlist(&m_std_cells),
    m_placement_library(&m_std_cells),
    m_placement(&m_netlist, &m_placement_library)
{

}

application::~application()
{

}

bool application::read_lefdef(const std::string &LEF, const std::string &DEF)
{
    parsing::lef lef(LEF);
    parsing::def def(DEF);

    placement::def2placement(def, m_placement);
    placement::lef2library(lef, m_placement_library);

    return true;
}

std::vector<std::pair<entity::entity, geometry::multi_polygon<geometry::polygon<geometry::point<double> > > > > application::cells_geometries() const
{
    std::vector<std::pair<entity::entity, geometry::multi_polygon<geometry::polygon<geometry::point<double> > > > > geometries;
    for(auto cell : m_netlist.cell_system())
        geometries.push_back({cell.first, m_placement.cell_geometry(cell.first)});
    return geometries;
}

}
