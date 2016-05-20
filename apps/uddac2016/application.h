#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>

#include "../placement/placement.h"
#include "../floorplan/floorplan.h"

#include "../timing/library.h"

#include "../timing/generic_sta.h"
#include "../timing/ceff.h"

#include "../interconnection/rc_tree.h"
#include "../timing-driven_placement/flute_rc_tree_estimation.h"


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



    ophidian::timingdriven_placement::flute_rc_tree_creator m_flute;

    ophidian::timing::design_constraints m_dc;
    std::unique_ptr<ophidian::timing::library_timing_arcs> m_tarcs;
    std::unique_ptr<ophidian::timing::library> m_timing_library;
    std::unique_ptr< ophidian::entity_system::vector_property< ophidian::interconnection::packed_rc_tree > >  m_rc_trees;
    std::unique_ptr< ophidian::timing::graph > m_graph;
    std::unique_ptr< ophidian::timing::timing_data > m_timing_data;
    std::unique_ptr< ophidian::timing::graph_and_topology> m_topology;
    std::unique_ptr< ophidian::timing::generic_sta<ophidian::timing::effective_capacitance_wire_model, ophidian::timing::pessimistic> > m_STA;
    std::set< ophidian::entity_system::entity> m_dirty_nets;

    std::unique_ptr<SA> m_sa;
public:
    application();
    virtual ~application();

    void make_cell_nets_dirty(ophidian::entity_system::entity cell);
    void update_dirty_rc_trees();

    bool read_lefdef(const std::string & LEF, const std::string & DEF);
    bool read_def(const std::string & DEF);
    bool read_verilog(const std::string & v);
    void read_tau2014_lib(const std::string & file);
    void read_liberty(const std::string & file);


    bool STA() const {
        return m_STA != nullptr;
    }

    const std::vector<ophidian::entity_system::entity> &  net_pins(const ophidian::entity_system::entity & net) const {
        return m_netlist->net_pins(net);
    }

    const ophidian::geometry::point<double> pin_position(const ophidian::entity_system::entity & pin) const {
        return m_placement->pin_position(pin);
    }

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


    double placement_units2micron() const {
        return m_placement_library->dist2microns();
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

    void cell_std_cell(const ophidian::entity_system::entity & cell, const ophidian::entity_system::entity & std_cell);

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

    double cell_worst_slack(const ophidian::entity_system::entity & cell) const
    {
        double worst = std::numeric_limits<double>::infinity();
        if(!m_STA)
            return worst;
        auto & cell_pins = m_netlist->cell_pins(cell);
        for(auto pin : cell_pins)
            worst = std::min(worst, std::min(m_STA->rise_slack(pin).value(),m_STA->fall_slack(pin).value()));
        return worst;
    }


    std::unordered_map<ophidian::entity_system::entity, std::pair<double, bool> > worst_slacks() const {
        std::unordered_map<ophidian::entity_system::entity, std::pair<double, bool> > worst_slacks;
        worst_slacks.reserve(m_netlist->cell_count());
        for(auto cell : m_netlist->cell_system())
            worst_slacks.insert(std::make_pair(cell, std::make_pair(cell_worst_slack(cell), m_placement->cell_fixed(cell))));
        return worst_slacks;
    }

    const ophidian::entity_system::entity_system & nets() const {
        return m_netlist->net_system();
    }

    const std::string net_name(const ophidian::entity_system::entity & net) const {
        return m_netlist->net_name(net);
    }

    void run_SA();
    void run_STA();

    void legalize();
};

class SA {
    application & m_app;
    double m_T{1024.0};
    std::uniform_real_distribution<double> m_dist{0.0, 1.0};
    std::default_random_engine m_engine;
public:
    SA(application & app);
    void run_it();
};

}

#endif // APPLICATION_H
