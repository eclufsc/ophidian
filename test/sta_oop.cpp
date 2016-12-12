#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <array>
#include <fstream>
#include <iostream>

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
    double input_slew, out_capacitance, delay;
};

//for sake of simplicity, the interpolation process was omitted
double process_oo(LookUpTable & lut, double & slew_ref,double & capacitance_ref)
{
    auto i = std::distance(lut.slew_index.begin(), std::lower_bound(lut.slew_index.begin(), lut.slew_index.end(), slew_ref));

    auto j = std::distance(lut.capacitance_index.begin(), std::lower_bound(lut.capacitance_index.begin(), lut.capacitance_index.end(), capacitance_ref));

    return lut.values[i*8+j];
}

int main(){

    std::array<double, 4096> lut_input;
    std::array<int, 2048> ports_type;

    std::ifstream ifs("input_files/lut_input");
    for(auto & input: lut_input){
        ifs >> input;
    }
    ifs.close();

    std::ifstream ifs2("input_files/ports_types");
    for(auto & type: ports_type){
        ifs2 >> type;
    }
    ifs2.close();

    std::array<LookUpTable, 211> cell_library;
    std::array<std::pair<CircuitCell,double>, 2048> logic_cells;

    //set the circuit's cells type
    for(unsigned int i = 0; i < logic_cells.size();++i){
        logic_cells.at(i).second = ports_type.at(i);
    }

    //set the input slews and capacitances
    unsigned int i, j;
    for(i = 0, j=0; i < logic_cells.size();++i, j+=2){
        logic_cells.at(i).first.input_slew = lut_input.at(j);
        logic_cells.at(i).first.out_capacitance = lut_input.at(j+1);
    }

    //process ports
    for(auto p : logic_cells){
        p.first.delay = process_oo(cell_library.at(p.second), p.first.input_slew, p.first.out_capacitance);
    }

    return 0;
}
