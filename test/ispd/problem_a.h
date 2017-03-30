#ifndef ophidian_PROBLEM_A_H
#define ophidian_PROBLEM_A_H

#include "../src/floorplan/floorplan.h"
#include "../src/floorplan/lefdef2floorplan.h"

using namespace ophidian;

namespace problem_a{
class Pin{
public:
    Pin(){}
};

class StdCell{
public:
    StdCell(){}
};

class Cell{
    std::string m_name;
    StdCell * m_standard_cell;
    std::vector<Pin*> m_pins;
public:
    Cell(const std::string & name, std::size_t pins_size): m_name(name){m_pins.reserve(pins_size);}
};

class PlacementCell : public Cell{
    using point = geometry::point<double>;
    using polygon = geometry::polygon<point>;
    using multipolygon = geometry::multi_polygon<polygon>;

    multipolygon m_geometry;
    point m_position;
    bool m_fixed;
public:
    PlacementCell(const std::string & name, const bool fixed, std::size_t pins_size):Cell(name, pins_size), m_fixed(m_fixed){}
    void set_geometry(multipolygon geometry){m_geometry = geometry;}
    void set_position(const point & position){m_position = position;}
    const point & get_position(){return m_position;}
};

void problem_a_ood(parsing::def * def, parsing::lef * lef, placement::placement & m_placement, Metric &metric){
   floorplan::floorplan m_floorplan;
   floorplan::lefdef2floorplan(*lef, *def, m_floorplan);
   geometry::point<double> m_chip_boundaries = m_floorplan.chip_boundaries();
   std::vector<PlacementCell> m_cells;
   m_cells.reserve(m_placement.netlist().cell_system().size());

   for(auto cell : m_placement.netlist().cell_system()){
     PlacementCell p_cell(m_placement.netlist().cell_name(cell), m_placement.cell_fixed(cell), m_placement.netlist().cell_pins(cell).size());
     p_cell.set_geometry(m_placement.cell_geometry(cell));
     p_cell.set_position(m_placement.cell_position(cell));
     m_cells.push_back(p_cell);
   }

   bool placemente_is_legal = true;
   metric.start();
   for(auto cell : m_cells){
     auto position = cell.get_position();
     if(position.x() < 0 || position.y() < 0 || position.x() > m_chip_boundaries.x() || position.y() > m_chip_boundaries.y()){
       placemente_is_legal = false;
       break;
       }
     }
   metric.end();
   metric.print_result();
}

void problem_a_dod(parsing::def * def, parsing::lef * lef, placement::placement & m_placement, Metric &metric){
   floorplan::floorplan m_floorplan;
   floorplan::lefdef2floorplan(*lef, *def, m_floorplan);
   geometry::point<double> m_chip_boundaries = m_floorplan.chip_boundaries();
   bool placement_is_legal = true;
   metric.start();
   for(auto position : m_placement.cell_properties().positions()){
     if(position.x() < 0 || position.y() < 0 || position.x() > m_chip_boundaries.x() || position.y() > m_chip_boundaries.y()){
       placement_is_legal = false;
       break;
     }
   }
   metric.end();
   metric.print_result();
}
}

#endif //ophidian_PROBLEM_A_H
