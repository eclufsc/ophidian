#include <iostream>
#include <array>
#include <fstream>
#include <algorithm>
#include <chrono>

struct LookUpTable {
    LookUpTable():slew_index{{0.8, 2.3, 3.6, 4.1, 6.0, 8.5, 10.0}},
                  capacitance_index{{1.2, 2.7, 3.4, 4.2, 5.6, 7.8, 8.1, 10.0}},
                  values{{1,  2,  3,  4,  5,  6,  7,  8,
                          9, 10, 11, 12, 13, 14, 15, 16,
                         17, 18, 19, 20, 21, 22, 23, 24,
                         25, 26, 27, 28, 29, 30, 31, 32,
                         33, 34, 35, 36, 37, 38, 39, 40,
                         41, 42, 43, 44, 45, 46, 47, 48,
                         49, 50, 51, 52, 53, 54, 55, 56}}//to acess all elements[i*8+j]
    {}
    std::array<double, 7> slew_index;//i
    std::array<double, 8> capacitance_index;//j
    std::array<double, 56> values;
};

struct CircuitCell {
    CircuitCell(){}
    double input_slew, out_capacitance, i_index, j_index, delay;
};

//for sake of simplicity, the interpolation process was omitted
double process_oo(const LookUpTable & lut,const  double & slew_ref,const double & capacitance_ref)
{
    auto i = std::distance(lut.slew_index.begin(), std::lower_bound(lut.slew_index.begin(), lut.slew_index.end(), slew_ref));

    auto j = std::distance(lut.capacitance_index.begin(), std::lower_bound(lut.capacitance_index.begin(), lut.capacitance_index.end(), capacitance_ref));

    return lut.values.at(i*8+j);
}

template <std::size_t INDEX>
double find_index(const std::array<double, INDEX>& index_table, const double & ref)
{
    return std::distance(index_table.begin(), std::lower_bound(index_table.begin(), index_table.end(), ref));
}

int main(){
    const unsigned int num_elements = 4096;
    const unsigned int num_inputs = 8192;
    const unsigned int num_library_cells = 211;

    std::array<double, num_inputs> lut_input;
    std::array<int, num_elements> ports_type;

    std::ifstream ifs("input_files/lut_input");
    for(auto & input: lut_input)
        ifs >> input;
    ifs.close();

    std::ifstream ifs2("input_files/ports_types");
    for(auto & type: ports_type)
        ifs2 >> type;
    ifs2.close();

    //order ports by type
    auto order_time_start = std::chrono::high_resolution_clock::now();
    std::sort(ports_type.begin(), ports_type.end());
    auto order_time_end = std::chrono::high_resolution_clock::now();
    auto order_time = order_time_end - order_time_start;

    std::array<LookUpTable, num_library_cells> cell_library;
    std::array<std::pair<CircuitCell,double>, num_elements> logic_cells;

    //set the circuit's cells type
    for(unsigned int i = 0; i < logic_cells.size();++i)
        logic_cells.at(i).second = ports_type.at(i);

    //set the input slews and capacitances
    unsigned int i, j;
    for(i = 0, j=0; i < logic_cells.size(); ++i, j+=2){
        logic_cells.at(i).first.input_slew = lut_input.at(j);
        logic_cells.at(i).first.out_capacitance = lut_input.at(j+1);
    }

    //Object Oriented
    //process ports
    for(auto p : logic_cells){
        auto std_cell = cell_library.at(p.second);
        p.first.delay = process_oo(std_cell, p.first.input_slew, p.first.out_capacitance);
    }


    /*
    //Data Oriented Design
    for(auto p : logic_cells){
        auto std_cell = cell_library.at(p.second);

        p.first.i_index = find_index(std_cell.slew_index, p.first.input_slew);

        p.first.j_index = find_index(std_cell.capacitance_index, p.first.out_capacitance);

        p.first.delay = std_cell.values.at(p.first.i_index * 8 + p.first.j_index);
    }*/

    return 0;
}
