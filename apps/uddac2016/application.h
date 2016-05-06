#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>

#include "../placement/placement.h"

namespace uddac2016 {

class application
{
    ophidian::standard_cell::standard_cells m_std_cells;
    ophidian::netlist::netlist m_netlist;
    ophidian::placement::library m_placement_library;
    ophidian::placement::placement m_placement;
public:
    application();
    virtual ~application();

    bool read_lefdef(const std::string & LEF, const std::string & DEF);
    bool read_def(const std::string & DEF);

    std::vector< std::pair<ophidian::entity::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > cells_geometries() const;
    std::string cell_name(const ophidian::entity::entity & cell_name) const {
        return m_netlist.cell_name(cell_name);
    }
};

}

#endif // APPLICATION_H
