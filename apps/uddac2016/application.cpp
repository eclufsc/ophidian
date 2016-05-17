#include "application.h"

#include "../placement/def2placement.h"
#include "../placement/lef2library.h"
#include "../floorplan/lefdef2floorplan.h"
#include "../interconnection/hpwl.h"

#include "../parsing/verilog.h"
#include "../netlist/verilog2netlist.h"


#include "../placement/hpwl.h"

#include "../placement/legalization/abacus/abacus.h"

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
    return true;
}

bool application::read_verilog(const std::string &v)
{
    parsing::verilog verilog(v);
    netlist::verilog2netlist(verilog, *m_netlist);
    return true;
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
}

void application::run_SA()
{
    if(!m_sa)
        m_sa.reset(new SA(*this));
    m_sa->run_it();
}

void application::legalize()
{
    placement::legalization::abacus::abacus abacus(*m_floorplan, *m_placement);
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
    std::cout << "HPWL before " << HPWL.value() << std::endl;

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

        std::uniform_real_distribution<double> x_dist(std::max(pos0.x()-10000, m_app.m_floorplan->chip_origin().x()), std::min(m_app.m_floorplan->chip_boundaries().x()-cell_width, pos0.x()+10000));
        std::uniform_real_distribution<double> y_dist(std::max(pos0.y()-10000, m_app.m_floorplan->chip_origin().y()), std::min(m_app.m_floorplan->chip_boundaries().y()-cell_height, pos0.y()+10000));

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
            accepted_moves++;
        }
        ++i;
    }
    m_T *= .8;

    placement::hpwl HPWLF(*m_app.m_placement);
    std::cout << "HPWL after " << HPWLF.value() << std::endl;
}

}
