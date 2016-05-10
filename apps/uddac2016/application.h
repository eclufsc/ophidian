#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>

#include "../placement/placement.h"
#include "../floorplan/floorplan.h"

namespace uddac2016 {

class SA;
class application
{
    friend class SA;
    ophidian::standard_cell::standard_cells m_std_cells;
    ophidian::netlist::netlist m_netlist;
    ophidian::placement::library m_placement_library;
    ophidian::placement::placement m_placement;
    ophidian::floorplan::floorplan m_floorplan;

    std::unique_ptr<SA> m_sa;
public:
    application();
    virtual ~application();

    bool read_lefdef(const std::string & LEF, const std::string & DEF);
    bool read_def(const std::string & DEF);

    std::vector< std::pair<ophidian::entity::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > cells_geometries() const;
    ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > cell_geometry(const ophidian::entity::entity & cell) const;
    void cell_position(const ophidian::entity::entity & cell, const ophidian::geometry::point<double> & p);

    std::array<ophidian::geometry::point<double>, 4> chip_boundaries() const {
        std::array<ophidian::geometry::point<double>, 4> result;
        result[0] = m_floorplan.chip_origin();
        result[1].x(m_floorplan.chip_origin().x()); result[1].y(m_floorplan.chip_boundaries().y());
        result[2] = m_floorplan.chip_boundaries();
        result[3].x(m_floorplan.chip_boundaries().x()); result[3].y(m_floorplan.chip_origin().y());
        return result;
    }



    bool cell_is_fixed(const ophidian::entity::entity & cell) const {
        return m_placement.cell_fixed(cell);
    }

    std::string cell_name(const ophidian::entity::entity & cell_name) const {
        return m_netlist.cell_name(cell_name);
    }

    const ophidian::geometry::point<double> cell_position(const ophidian::entity::entity & cell) const {
        return m_placement.cell_position(cell);
    }


    void run_SA(const std::string &verilog_file);
};

class SA {
    application & m_app;
    double m_T{10000.0};
    std::uniform_real_distribution<double> m_dist{0.0, 1.0};
    std::default_random_engine m_engine;
public:
    SA(application & app, const std::string &verilog_file);
    void run_it();
};

}

#endif // APPLICATION_H
