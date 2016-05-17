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

    std::unique_ptr<ophidian::standard_cell::standard_cells> m_std_cells;
    std::unique_ptr<ophidian::netlist::netlist> m_netlist;
    std::unique_ptr<ophidian::placement::library> m_placement_library;
    std::unique_ptr<ophidian::placement::placement> m_placement;
    std::unique_ptr<ophidian::floorplan::floorplan> m_floorplan;

    std::unique_ptr<SA> m_sa;
public:
    application();
    virtual ~application();

    bool read_lefdef(const std::string & LEF, const std::string & DEF);
    bool read_def(const std::string & DEF);
    bool read_verilog(const std::string & v);

    std::vector< std::pair<ophidian::entity_system::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > cells_geometries() const;
    ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > cell_geometry(const ophidian::entity_system::entity & cell) const;
    void cell_position(const ophidian::entity_system::entity & cell, const ophidian::geometry::point<double> & p);

    std::array<ophidian::geometry::point<double>, 4> chip_boundaries() const {
        std::array<ophidian::geometry::point<double>, 4> result;
        result[0] = m_floorplan->chip_origin();
        result[1].x(m_floorplan->chip_origin().x()); result[1].y(m_floorplan->chip_boundaries().y());
        result[2] = m_floorplan->chip_boundaries();
        result[3].x(m_floorplan->chip_boundaries().x()); result[3].y(m_floorplan->chip_origin().y());
        return result;
    }


    ophidian::entity_system::entity std_cell_find(const std::string & name) const {
        return m_std_cells->cell_create(name);
    }



    bool cell_is_fixed(const ophidian::entity_system::entity & cell) const {
        return m_placement->cell_fixed(cell);
    }

    std::string cell_name(const ophidian::entity_system::entity & cell) const {
        return m_netlist->cell_name(cell);
    }

    ophidian::entity_system::entity cell_std_cell(const ophidian::entity_system::entity & cell) const {
        return m_netlist->cell_std_cell(cell);
    }

    void cell_std_cell(const ophidian::entity_system::entity & cell, const ophidian::entity_system::entity & std_cell) const {
        m_netlist->cell_std_cell(cell, std_cell);
    }

    const ophidian::geometry::point<double> cell_position(const ophidian::entity_system::entity & cell) const {
        return m_placement->cell_position(cell);
    }

    std::size_t cell_count() const {
        return m_netlist->cell_count();
    }

    std::size_t pin_count() const {
        return m_netlist->pin_count();
    }

    std::size_t net_count() const {
        return m_netlist->net_count();
    }

    std::string circuit_name() const {
        return m_netlist->module_name();
    }

    const std::string std_cell_name(const ophidian::entity_system::entity & std_cell) const {
        return m_std_cells->cell_name(std_cell);
    }

    std::size_t std_cell_pin_count(const ophidian::entity_system::entity & std_cell) const {
        return m_std_cells->cell_pins(std_cell).size();
    }


    const ophidian::standard_cell::standard_cells& std_cells() const {
        return *m_std_cells;
    }


    void run_SA();

    void legalize();
};

class SA {
    application & m_app;
    double m_T{2048.0};
    std::uniform_real_distribution<double> m_dist{0.0, 1.0};
    std::default_random_engine m_engine;
public:
    SA(application & app);
    void run_it();
};

}

#endif // APPLICATION_H
