#include "linear_library.h"

#include <fstream>
#include <cassert>

namespace ophidian {
namespace parsing {
namespace tau2014 {

linear_library::linear_library(const std::string &filename)
{
    std::ifstream in(filename);
    std::string temp_str;
    double temp_double;

    in >> temp_str;
    std::string current_cell_id;
    while(!in.eof())
    {
        if(temp_str == "metal")
        {
            in >> temp_double;
            in >> temp_double;
            in >> temp_double;
        }
        else if(temp_str == "cell")
        {
            in >> current_cell_id;
            cell c;
            c.name = current_cell_id;
            m_cells[current_cell_id] = c;
        } else if(temp_str == "pin")
        {
            pin p;
            in >> p.name;
            in >> p.direction;
            cell & current_cell = m_cells[current_cell_id];
            if(p.direction == "input" || p.direction == "clock")
            {
                if(p.direction == "clock")
                {
                    p.clock = true;
                    current_cell.clock_pin_id = current_cell.input_pins.size();
                }
                p.direction = "input";
                in >> p.rise_capacitance;
                in >> p.fall_capacitance;
                current_cell.input_pins.push_back(p);
            } else if(p.direction == "output"){
                current_cell.output_pins.push_back(p);
            }
        } else if(temp_str == "timing")
        {
            in >> temp_str;
            arc a;
            cell & current_cell = m_cells[current_cell_id];
            for(std::size_t i{0}; i < current_cell.input_pins.size(); ++i)
            {
                if(current_cell.input_pins[i].name == temp_str)
                {
                    a.from = i;
                    break;
                }
            }
            in >> temp_str;
            for(std::size_t i{0}; i < current_cell.output_pins.size(); ++i)
            {
                if(current_cell.output_pins[i].name == temp_str)
                {
                    a.to = i;
                    break;
                }
            }

            in >> a.unateness;
            in >> a.fall_slew[0];
            in >> a.fall_slew[1];
            in >> a.fall_slew[2];
            for(int i{3}; i < 9; ++i)
                in >> temp_double;
            in >> a.rise_slew[0];
            in >> a.rise_slew[1];
            in >> a.rise_slew[2];
            for(int i{3}; i < 9; ++i)
                in >> temp_double;
            in >> a.fall_delay[0];
            in >> a.fall_delay[1];
            in >> a.fall_delay[2];
            for(int i{3}; i < 9; ++i)
                in >> temp_double;
            in >> a.rise_delay[0];
            in >> a.rise_delay[1];
            in >> a.rise_delay[2];
            for(int i{3}; i < 9; ++i)
                in >> temp_double;

            current_cell.arcs[current_cell.input_pins[a.from].name][current_cell.output_pins[a.to].name] = a;
        } else if(temp_str == "setup" || temp_str == "hold")
        {
            cell & current_cell = m_cells[current_cell_id];
            test t;
            t.setup_or_hold = temp_str;
            in >> temp_str;
            for(std::size_t i{0}; i < current_cell.input_pins.size(); ++i)
            {
                if(current_cell.input_pins[i].name == temp_str)
                {
                    t.from = i;
                    current_cell.clock_pin_id = i;
                    break;
                }
            }
            in >> temp_str;
            for(std::size_t i{0}; i < current_cell.input_pins.size(); ++i)
            {
                if(current_cell.input_pins[i].name == temp_str)
                {
                    t.to = i;
                    current_cell.data_pin_id = i;
                    break;
                }
            }
            in >> t.type;
            in >> t.fall[0];
            in >> t.fall[1];
            in >> t.fall[2];

            in >> t.rise[0];
            in >> t.rise[1];
            in >> t.rise[2];

            if(t.setup_or_hold == "setup")
                current_cell.setup.push_back(t);
            else if(t.setup_or_hold == "hold")
                current_cell.hold.push_back(t);
        }

        in >> temp_str;
    }

    //    if(temp_str == "cell")
    //    {
    //        cell c;
    //        in >> c.name;
    //        in >> temp_str;
    //        while(temp_str == "pin")
    //        {
    //            pin p;
    //            in >> p.name;
    //            in >> p.direction;
    //            if(p.direction == "input")
    //            {
    //                in >> p.rise_capacitance;
    //                in >> p.fall_capacitance;
    //                c.input_pins.push_back(p);
    //            } else if(p.direction == "output")
    //            {
    //                in >> temp_str;
    //                while(temp_str == "timing")
    //                {

    //                }
    //            }
    //            in >> temp_str;
    //        }




    //    }

}

linear_library::~linear_library()
{

}

}
}
}
