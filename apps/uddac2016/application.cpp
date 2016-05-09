#include "application.h"

#include "../placement/def2placement.h"
#include "../placement/lef2library.h"
#include "../floorplan/lefdef2floorplan.h"
#include "../interconnection/hpwl.h"

#include "../parsing/verilog.h"
#include "../netlist/verilog2netlist.h"

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
    floorplan::lefdef2floorplan(lef,def,m_floorplan);

    return true;
}

bool application::read_def(const std::string &DEF)
{
    parsing::def def(DEF);
    placement::def2placement(def, m_placement);
    return true;
}

std::vector<std::pair<entity::entity, geometry::multi_polygon<geometry::polygon<geometry::point<double> > > > > application::cells_geometries() const
{
    std::vector<std::pair<entity::entity, geometry::multi_polygon<geometry::polygon<geometry::point<double> > > > > geometries;
    for(auto cell : m_netlist.cell_system())
        geometries.push_back({cell.first, m_placement.cell_geometry(cell.first)});
    return geometries;
}

ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > application::cell_geometry(const entity::entity &cell) const
{
    return m_placement.cell_geometry(cell);
}

void application::cell_position(const entity::entity &cell, const ophidian::geometry::point<double> &p)
{
    m_placement.cell_position(cell, p);
}

void application::run_SA(const std::string &verilog_file)
{
    parsing::verilog verilog(verilog_file);
    netlist::verilog2netlist(verilog, m_netlist);



    std::uniform_real_distribution<double> SA_dist(0.0, 1.0);
    std::default_random_engine engine;


    double T = 10000.0;
    int accepted_moves = 0;

    std::size_t i = 0;

    for(int it = 0; it < 5; ++it)
    {
        std::cout << "it " << it << " T " << T << std::endl;
        for(auto cell : m_netlist.cell_system())
        {

            auto & cell_pins = m_netlist.cell_pins(cell.first);

            double HPWL0 = 0.0;

            for(auto pin : cell_pins)
            {
                auto net = m_netlist.pin_net(pin);
                auto & net_pins = m_netlist.net_pins(net);
                std::vector< geometry::point<double> > pin_positions(net_pins.size());
                for(std::size_t i = 0; i < net_pins.size(); ++i)
                    pin_positions[i] = m_placement.pin_position(net_pins[i]);
                HPWL0 += interconnection::hpwl(pin_positions);
            }

            geometry::point<double> pos0 = m_placement.cell_position(cell.first);

            std::uniform_real_distribution<double> x_dist(pos0.x()-10000, pos0.x()+10000);
            std::uniform_real_distribution<double> y_dist(pos0.y()-10000, pos0.y()+10000);

            geometry::point<double> position(x_dist(engine), y_dist(engine));
            m_placement.cell_position(cell.first, position);

            double HPWLF = 0.0;
            for(auto pin : cell_pins)
            {
                auto net = m_netlist.pin_net(pin);
                auto & net_pins = m_netlist.net_pins(net);
                std::vector< geometry::point<double> > pin_positions(net_pins.size());
                for(std::size_t i = 0; i < net_pins.size(); ++i)
                    pin_positions[i] = m_placement.pin_position(net_pins[i]);
                HPWLF += interconnection::hpwl(pin_positions);
            }

            double Delta = HPWLF-HPWL0;

            //        std::cout << "trying cell " << m_netlist.cell_name(cell.first) << std::endl;
            //        std::cout << "HPWL0 " << HPWL0 << " HPWLF " << HPWLF << " Delta " << Delta << std::endl;
            //        std::cout << "T " << T << std::endl;


            bool accepted = false;
            if(Delta > 0)
            {
                //            std::cout << "prob: " << exp(-Delta/T) << std::endl;
                double rand = SA_dist(engine);
                //            std::cout << "roulette: " << rand << std::endl;

                if(rand < exp(-Delta/T))
                    accepted = true;

            } else
                accepted = true;

            if(!accepted)
            {
                //            std::cout << " REJECTED!!" << std::endl;
                m_placement.cell_position(cell.first, pos0);
            }
            else
            {
                //            std::cout << " ACCEPTED!!" << std::endl;
                accepted_moves++;
            }
            ++i;
        }
        T *= .8;
    }
    std::cout << "accepted " << accepted_moves << " moves" << std::endl;
}

}
