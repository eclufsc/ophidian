#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <array>
#include <iostream>
#include <fstream>

using namespace ophidian;
template<class K, class V>
using Property = entity_system::Property<K,V>;

struct LUT : public entity_system::EntityBase {
    using EntityBase::EntityBase;
};

template <class Entity_, class InputValue, class OutputValue, class InputCollection>
void findIndices(const entity_system::EntitySystem<Entity_> & sys, const Property<Entity_, InputValue>& inputReferences, const Property<Entity_, InputCollection>& input, Property<Entity_, OutputValue> & output) 
{
    auto enIt = sys.begin();
    auto refIt = inputReferences.begin();
    auto inIt = input.begin();
    auto outIt = output.begin();
    
    for(; enIt != sys.end() && refIt != inputReferences.end() && inIt != input.end() && outIt != output.end(); ++enIt, ++refIt, ++inIt, ++outIt) {
        (*outIt) = std::distance(inIt->begin(), std::lower_bound(inIt->begin(), inIt->end(), *refIt));
    }
}


double findValue(std::array<std::array<double,8>, 7>> & lut, int i, int j){


int main(int argc, char *argv[])
{
    entity_system::EntitySystem<LUT> sys;
    sys.reserve(2048);
    for(int i = 0; i < 2048; i++){
        sys.add();
    }
    
    auto slew_indexes = entity_system::Property<LUT, std::array<double, 7>>(sys);
    for(auto & index: slew_indexes){
        index = {0.8,2.3,3.6,4.1,6.0,8.5,10.0};
    }
    
    auto capacitance_indexes = entity_system::Property<LUT, std::array<double, 8>>(sys);
    for(auto & index: capacitance_indexes){
        index = {1.2,2.7,3.4,4.2,5.6,7.8,8.1,10.0};
    }

    auto i_entries = entity_system::Property<LUT, double>(sys);
    auto j_entries = entity_system::Property<LUT, double>(sys);
    {
        std::ifstream ifs("input_files/lut_input");
        auto it = i_entries.begin();
        auto jt = j_entries.begin();
        for(; it != i_entries.end() && jt != j_entries.end(); ++it,++jt){
            ifs >> *it;
            ifs >> *jt;
        }
        ifs.close();
    }

    auto i_indices = entity_system::Property<LUT, std::array<double, 7>::size_type>(sys);
    auto j_indices = entity_system::Property<LUT, std::array<double, 8>::size_type>(sys);

    findIndices(sys, i_entries, slew_indexes, i_indices); 
    findIndices(sys, j_entries, capacitance_indexes, j_indices); 
    
    auto look_up_tables = entity_system::Property<LUT, std::array<std::array<double, 8>, 7>>(sys);
    for(auto & lut: look_up_tables){
        lut = { { 0.0,  1.0,  2.0,  3.0,  4.0,  5.0,  6.0,  7.0},
                { 8.0,  9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0},
                {16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0, 23.0},
                {24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0},
                {32.0, 33.0, 34.0, 35.0, 36.0, 37.0, 38.0, 39.0},
                {40.0, 41.0, 42.0, 43.0, 44.0, 45.0, 46.0, 47.0},
                {48.0, 49.0, 50.0, 51.0, 52.0, 53.0, 54.0, 55.0} };
    }

    auto values = 

    return 0;
}
