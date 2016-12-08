#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <array>
#include <fstream>
#include <iostream>


template<class T, std::size_t I, std::size_t J>
struct LookUpTable {
    LookUpTable(std::array<T, I> & slew_index, std::array<T, J> & capacitance_index, std::array<std::array<T, J>, I> & values):
        slew_index(slew_index),
        capacitance_index(capacitance_index),
        values(values)
    {    
    }
    
    LookUpTable(){}
    
    std::array<T, I> slew_index;
    std::array<T, J> capacitance_index;
    std::array<std::array<T, J>, I> values;
};

template<class T, std::size_t I, std::size_t J>
T findValue(LookUpTable<T, I, J> & lut, T & slew_ref, T & capacitance_ref) 
{
    auto i = std::distance(lut.slew_index.begin(), std::lower_bound(lut.slew_index.begin(), lut.slew_index.end(), slew_ref));

    auto j = std::distance(lut.capacitance_index.begin(), std::lower_bound(lut.capacitance_index.begin(), lut.capacitance_index.end(), capacitance_ref));

    return lut.values[i][j];
}

LookUpTable<double,7,8> createLUT_double7x8(){

    std::array<double, 7> slew_index = {0.8,2.3,3.6,4.1,6.0,8.5,10.0};
    std::array<double, 8> capacitance_index = {1.2,2.7,3.4,4.2,5.6,7.8,8.1,10.0};
    std::array<std::array<double, 8>, 7> values;

    double fill_with = 0.0;
    for(auto & row: values){
        for(auto & value: row){
            value = fill_with;
            fill_with++;
        }
    }

    return LookUpTable<double,7,8>{slew_index, capacitance_index, values};
}

int main(){
    
    std::array<double, 4096> lut_input;
    std::ifstream ifs("input_files/lut_input");
    for(auto & input: lut_input){
        ifs >> input;
    }
    ifs.close();

    std::array<LookUpTable<double, 7, 8>, 2048> look_up_tables;
    for(auto & lut: look_up_tables){
        lut = createLUT_double7x8();
    }

    std::array<double, 2048> values;

    auto itVal = values.begin();
    auto itIn = lut_input.begin();
    auto itLut = look_up_tables.begin();
    for(; itIn != lut_input.end() && itLut != look_up_tables.end() && itVal != values.end() ; itIn += 2, ++itLut, ++itVal ){
        (*itVal) = findValue(*itLut, *itIn, *(itIn + 1));
    }

    for(const auto & value: values){
        std::cout << value << std::endl;
    }
    return 0;
}
