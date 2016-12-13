#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/entity_system/Aggregation.h>
#include <array>
#include <fstream>

using namespace ophidian;
template<class K, class V>
using Property = entity_system::Property<K,V>;

struct LUT : public entity_system::EntityBase {
    using EntityBase::EntityBase;
};

struct Cell : public entity_system::EntityBase {
    using EntityBase::EntityBase;
};

template<class EntityWhole,
         class EntityPart,
         class InputValue,
         class InputCollection,
         class OutputValue>
void find_indexes(const entity_system::EntitySystem<EntityWhole>& sys,
                  const entity_system::EntitySystem<EntityPart>& sys2,
                  const entity_system::Property<EntityPart, InputValue>& inputValue,
                  const entity_system::Property<EntityWhole, InputCollection>& inputCollection,
                  const entity_system::Aggregation<EntityWhole, EntityPart>& aggregation,
                  entity_system::Property<EntityPart, OutputValue>& outputValue){
for(auto cell : sys2){
    auto std_cell = aggregation.whole(cell);
    outputValue[cell] = std::distance(inputCollection[std_cell].begin(),
                                      std::lower_bound(inputCollection[std_cell].begin(),
                                                       inputCollection[std_cell].end(), inputValue[cell]));
}
}

template<class EntityWhole,
         class EntityPart,
         class InputI,
         class InputJ,
         class Values,
         class OutputValue>
void interpolation(const entity_system::EntitySystem<EntityWhole>& sys,
                   const entity_system::EntitySystem<EntityPart>& sys2,
                   const entity_system::Property<EntityPart, InputI>& inputI,
                   const entity_system::Property<EntityPart, InputJ>& inputJ,
                   const entity_system::Property<EntityWhole, Values>& values,
                   const entity_system::Aggregation<EntityWhole, EntityPart>& aggregation,
                   entity_system::Property<EntityPart, OutputValue>& outputValue){
    for(auto cell : sys2){
        auto std_cell = aggregation.whole(cell);
        auto i = inputI[cell];
        auto j = inputJ[cell];
        outputValue[cell] = values[std_cell].at(i*8+j);
    }
}

int main(int argc, char *argv[])
{
    //create a cell library
    entity_system::EntitySystem<LUT> cell_library;
    auto slew_index = entity_system::Property<LUT, std::array<double, 7>>(cell_library);
    auto capacitance_index = entity_system::Property<LUT, std::array<double, 8>>(cell_library);
    auto values = entity_system::Property<LUT, std::array<double, 56>>(cell_library);
    cell_library.reserve(211);
    for(unsigned int i = 0; i < 211; i++){
        auto std_cell = cell_library.add();
        slew_index[std_cell] = {0.8, 2.3, 3.6, 4.1, 6.0, 8.5, 10.0};
        capacitance_index[std_cell] = {1.2, 2.7, 3.4, 4.2, 5.6, 7.8, 8.1, 10.0};
        values[std_cell] = {1,  2,  3,  4,  5,  6,  7,  8,
                            9, 10, 11, 12, 13, 14, 15, 16,
                           17, 18, 19, 20, 21, 22, 23, 24,
                           25, 26, 27, 28, 29, 30, 31, 32,
                           33, 34, 35, 36, 37, 38, 39, 40,
                           41, 42, 43, 44, 45, 46, 47, 48,
                           49, 50, 51, 52, 53, 54, 55, 56};
    }

    std::array<double, 4096> lut_input;
    std::array<int, 2048> ports_type;
    std::ifstream ifs("input_files/lut_input");
    for(auto & input: lut_input)
        ifs >> input;
    ifs.close();
    std::ifstream ifs2("input_files/ports_types");
    for(auto & type: ports_type)
        ifs2 >> type;
    ifs2.close();

    //create circuit cells
    entity_system::EntitySystem<Cell> circuit_cells;
    auto input_slew = entity_system::Property<Cell, double>(circuit_cells);
    auto out_capacitance = entity_system::Property<Cell, double>(circuit_cells);
    auto slew_i = entity_system::Property<Cell, double>(circuit_cells);
    auto cap_j = entity_system::Property<Cell, double>(circuit_cells);
    auto delay = entity_system::Property<Cell, double>(circuit_cells);
    cell_library.reserve(2048);

    //set the input slews and capacitances
    unsigned int i, j;
    for(i = 0, j = 0; i < 2048; ++i, j+=2){
        auto cell = circuit_cells.add();
        input_slew[cell] = lut_input.at(j);
        out_capacitance[cell] = lut_input.at(j+1);
    }

    //bind a circuit cell to a library cell
   entity_system::Aggregation<LUT, Cell> aggregation(cell_library, circuit_cells);
   //aggregation.reserve(2048);//Can not be done, I don't know why :c
   for(unsigned int i = 0; i < 2048; ++i)
       aggregation.addAssociation(*std::next(cell_library.begin(), ports_type.at(i)),
                                  *std::next(circuit_cells.begin(), i));

   //find slew indexes
   find_indexes(cell_library, circuit_cells, input_slew, slew_index, aggregation, slew_i);

   //find cap indexes
   find_indexes(cell_library, circuit_cells, out_capacitance, capacitance_index, aggregation, cap_j);

   //do interpolation
   interpolation(cell_library, circuit_cells, slew_i, cap_j, values, aggregation, delay);

    return 0;
}
