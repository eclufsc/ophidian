#include "application.h"

#include "../placement/def2placement.h"
#include "../placement/lef2library.h"
#include "../floorplan/lefdef2floorplan.h"
#include "../interconnection/hpwl.h"

#include "../parsing/verilog.h"
#include "../netlist/verilog2netlist.h"


#include "../placement/hpwl.h"

#include "../placement/legalization/abacus/abacus.h"

#include "../parsing/tau2014/linear_library.h"
#include "../timing/tau2015lib2library.h"
#include "../timing/liberty.h"

#include "../timing/graph_builder.h"


namespace uddac2016 {

using namespace ophidian;


application::application() :
    m_std_cells(new standard_cell::standard_cells),
    m_netlist(new netlist::netlist{m_std_cells.get()}),
    m_placement_library(new placement::library(m_std_cells.get())),
    m_placement(new placement::placement(m_netlist.get(), m_placement_library.get()))
{
}

application::~application()
{

}

void application::make_cell_nets_dirty(entity_system::entity cell)
{
    auto &cell_pins = m_netlist->cell_pins(cell);
    for(auto pin : cell_pins)
        m_dirty_nets.insert(m_netlist->pin_net(pin));
}

void application::update_dirty_rc_trees()
{
    std::cout << "    creating rc_trees for dirty nets (" << m_dirty_nets.size() << ")..." << std::endl << std::flush;
    std::vector<entity_system::entity> nets; //to process in parallel
    nets.reserve(m_dirty_nets.size());
    nets.insert(nets.end(), m_dirty_nets.begin(), m_dirty_nets.end());

    std::size_t i;
#pragma omp parallel for shared(nets) private(i)
    for(i = 0; i < nets.size(); ++i)
    {
        auto & rc_tree_late = (*m_rc_trees)[m_netlist->net_system().lookup(nets[i])];
        auto & net_pins = m_netlist->net_pins(nets[i]);
        entity_system::entity source;
        for(auto pin : net_pins)
        {
            if(m_std_cells->pin_direction(m_netlist->pin_std_cell(pin)) == standard_cell::pin_directions::OUTPUT)
            {
                source = pin;
                break;
            }
        }
        assert( !(source == entity_system::entity{}) );
        interconnection::rc_tree tree;
        auto map = m_flute.create_tree(*m_placement, nets[i], tree, *m_timing_library);
        m_rc_trees->at(m_netlist->net_system().lookup(nets[i])) = tree.pack(map.at(source));
    }
#pragma omp barrier
    m_dirty_nets.clear();
    std::cout << "    creating rc_trees DONE!!" << std::endl << std::flush;
}

bool application::read_lefdef(const std::string &LEF, const std::string &DEF)
{
    parsing::lef lef(LEF);
    parsing::def def(DEF);

    m_std_cells.reset(new standard_cell::standard_cells);
    m_netlist.reset(new netlist::netlist{m_std_cells.get()});
    m_placement_library.reset(new placement::library{m_std_cells.get()});
    m_placement.reset(new placement::placement{m_netlist.get(), m_placement_library.get()});
    m_floorplan.reset(new floorplan::floorplan);
    m_sa.reset();

    placement::def2placement(def, *m_placement);
    placement::lef2library(lef, *m_placement_library);
    floorplan::lefdef2floorplan(lef,def,*m_floorplan);

    return true;
}

bool application::read_def(const std::string &DEF)
{
    parsing::def def(DEF);
    placement::def2placement(def, *m_placement);

    if(m_STA)
        for(auto cell : m_netlist->cell_system())
            make_cell_nets_dirty(cell);

    return true;
}

bool application::read_verilog(const std::string &v)
{
    parsing::verilog verilog(v);
    netlist::verilog2netlist(verilog, *m_netlist);
    return true;
}

void application::read_tau2014_lib(const std::string &file)
{
    parsing::tau2014::linear_library lib(file);
    m_tarcs.reset(new timing::library_timing_arcs(m_std_cells.get()));
    m_timing_library.reset(new timing::library(m_tarcs.get(), m_std_cells.get()));
    timing::tau2015lib2library(lib, *m_timing_library);
}

void application::read_liberty(const std::string &file)
{
    m_tarcs.reset(new timing::library_timing_arcs(m_std_cells.get()));
    m_timing_library.reset(new timing::library(m_tarcs.get(), m_std_cells.get()));
    timing::liberty::read(file, *m_timing_library);
}

std::vector<std::pair<entity_system::entity, geometry::multi_polygon<geometry::polygon<geometry::point<double> > > > > application::cells_geometries() const
{
    std::vector<std::pair<entity_system::entity, geometry::multi_polygon<geometry::polygon<geometry::point<double> > > > > geometries;
    for(auto cell : m_netlist->cell_system())
        geometries.push_back({cell, m_placement->cell_geometry(cell)});
    return geometries;
}

ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > application::cell_geometry(const entity_system::entity &cell) const
{
    return m_placement->cell_geometry(cell);
}

void application::cell_position(const entity_system::entity &cell, const ophidian::geometry::point<double> &p)
{
    m_placement->cell_position(cell, p);
    make_cell_nets_dirty(cell);
}

void application::cell_std_cell(const entity_system::entity &cell, const entity_system::entity &std_cell)
{
    m_netlist->cell_std_cell(cell, std_cell);
    if(m_graph)
    {
        timing::graph_builder::repower(*m_netlist, *m_timing_library, *m_graph, cell);
//        m_topology.reset(new timing::graph_and_topology(*m_graph, *m_netlist, *m_timing_library));
//        m_STA->topology(*m_topology);
        make_cell_nets_dirty(cell);
    }

}

void application::run_SA()
{
    if(!m_sa)
        m_sa.reset(new SA(*this));
    m_sa->run_it();
}

void application::run_STA()
{
    if(!m_graph)
    {
        m_graph.reset(new timing::graph);

        timing::default_design_constraints dc(*m_netlist);
        m_dc = dc.dc();
        timing::graph_builder::build(*m_netlist, *m_timing_library, m_dc, *m_graph);
        m_dc.clock.period = 80;
         for(auto driver : m_dc.input_drivers)
             m_std_cells->pin_direction(m_netlist->pin_std_cell(m_netlist->pin_by_name(driver.port_name)), standard_cell::pin_directions::OUTPUT);
         m_std_cells->pin_direction(m_netlist->pin_std_cell(m_netlist->pin_by_name(m_dc.clock.port_name)), standard_cell::pin_directions::OUTPUT);

         m_rc_trees.reset(new entity_system::vector_property< ophidian::interconnection::packed_rc_tree >());
         m_netlist->register_net_property(m_rc_trees.get());
         for(auto cell : m_netlist->cell_system())
             make_cell_nets_dirty(cell);

         m_timing_data.reset(new timing::timing_data(*m_timing_library, *m_graph));
         m_topology.reset(new timing::graph_and_topology(*m_graph, *m_netlist, *m_timing_library));
         m_STA.reset(new timing::generic_sta<timing::effective_capacitance_wire_model, timing::pessimistic>(*m_timing_data, *m_topology, *m_rc_trees));

         m_STA->set_constraints(m_dc);
    }

    update_dirty_rc_trees();
    std::cout << "updating ats..." << std::endl;
    m_STA->update_ats();
    std::cout << "updating rts..." << std::endl;
    m_STA->update_rts();


//    std::cout << "-- timing report" << std::endl;
//    for(auto pin : m_netlist->pin_system())
//    {
//        std::cout << "  ";
//        std::cout << m_netlist->pin_name(pin) << " ";
//        std::cout << "arrival rise " << m_STA->rise_arrival(pin) << " fall " << m_STA->fall_arrival(pin) << " ";
//        std::cout << "slew rise " << m_STA->rise_slew(pin) << " fall " << m_STA->fall_slew(pin) << " ";
//        std::cout << "slack rise " << m_STA->rise_slack(pin) << " fall " << m_STA->fall_slack(pin) << " ";
//        std::cout << std::endl;
//    }
}

void application::legalize()
{
    placement::legalization::abacus::abacus abacus(m_floorplan.get(), m_placement.get());
    abacus.legalize_placement();
}

SA::SA(application & app) :
    m_app(app)
{
}

void SA::run_it()
{
    int accepted_moves = 0;
    std::size_t i = 0;
    std::cout << "T " << m_T << std::endl;

    placement::hpwl HPWL(*m_app.m_placement);
//    std::cout << "HPWL before " << HPWL.value() << std::endl;

    for(auto cell : m_app.m_netlist->cell_system())
    {
        if(m_app.m_placement->cell_fixed(cell)) continue;
        auto & cell_pins = m_app.m_netlist->cell_pins(cell);

        double HPWL0 = 0.0;
        for(auto pin : cell_pins)
        {
            auto net = m_app.m_netlist->pin_net(pin);
            placement::hpwl net_hpwl(*m_app.m_placement, net);
            HPWL0 += net_hpwl.value();
        }

        geometry::point<double> pos0 = m_app.m_placement->cell_position(cell);

        geometry::box<geometry::point<double> > cell_bbox;
        geometry::envelope(m_app.m_placement->cell_geometry(cell), cell_bbox);

        const double cell_width = cell_bbox.max_corner().x()-cell_bbox.min_corner().x();
        const double cell_height = cell_bbox.max_corner().y()-cell_bbox.min_corner().y();
        const int WINDOW_SIZE = 100;

        std::uniform_real_distribution<double> x_dist(std::max(pos0.x()-WINDOW_SIZE*cell_width, m_app.m_floorplan->chip_origin().x()), std::min(m_app.m_floorplan->chip_boundaries().x()-cell_width, pos0.x()+WINDOW_SIZE*cell_width));
        std::uniform_real_distribution<double> y_dist(std::max(pos0.y()-WINDOW_SIZE*cell_height, m_app.m_floorplan->chip_origin().y()), std::min(m_app.m_floorplan->chip_boundaries().y()-cell_height, pos0.y()+WINDOW_SIZE*cell_height));

        geometry::point<double> position(x_dist(m_engine), y_dist(m_engine));
        m_app.m_placement->cell_position(cell, position);

        double HPWLF = 0.0;
        for(auto pin : cell_pins)
        {
            auto net = m_app.m_netlist->pin_net(pin);
            placement::hpwl net_hpwl(*m_app.m_placement, net);
            HPWLF += net_hpwl.value();
        }

        double Delta = HPWLF-HPWL0;

        //        std::cout << "trying cell " << m_app.m_netlist->cell_name(cell.first) << std::endl;
        //        std::cout << "HPWL0 " << HPWL0 << " HPWLF " << HPWLF << " Delta " << Delta << std::endl;
        //        std::cout << "T " << T << std::endl;


        bool accepted = false;
        if(Delta > 0)
        {
            //            std::cout << "prob: " << exp(-Delta/T) << std::endl;
            double rand = m_dist(m_engine);
            //            std::cout << "roulette: " << rand << std::endl;

            if(rand < exp(-Delta/m_T))
                accepted = true;

        } else
            accepted = true;

        if(!accepted)
        {
            //            std::cout << " REJECTED!!" << std::endl;
            m_app.m_placement->cell_position(cell, pos0);
        }
        else
        {
            //            std::cout << " ACCEPTED!!" << std::endl;

            m_app.make_cell_nets_dirty(cell);

            accepted_moves++;
        }
        ++i;
    }
    m_T *= .8;

    placement::hpwl HPWLF(*m_app.m_placement);
    std::cout << "HPWL " << 1e-6*(HPWLF.value()/m_app.m_placement_library->dist2microns()) << " m" << std::endl;
}

}
