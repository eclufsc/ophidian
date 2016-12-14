#include <iostream>
#include <array>
#include <fstream>
#include <ctime>

template<std::size_t SIZE,
         std::size_t SIZECELLS,
         std::size_t SIZEINDEX>
void find_indexes(const std::array<double, SIZE>& input_slew,
                  const std::array<double, SIZE>& type,
                  std::array<double, SIZE>& i_index,
                  const std::array<std::array<double, SIZEINDEX>, SIZECELLS>& slew_index){
    for(std::size_t i = 0; i < SIZE; ++i){
        auto std_cell_index = slew_index.at(type.at(i));
        i_index[i] = std::distance(std_cell_index.begin(),
                                   std::lower_bound(std_cell_index.begin(), std_cell_index.end(), input_slew.at(i)));
    }
}

template<std::size_t SIZE,
         std::size_t SIZECELLS>
void interpolation(const std::array<double, SIZE>& index_i,
                   const std::array<double, SIZE>& index_j,
                   const std::array<double, SIZE>& type,
                   std::array<double, SIZE>& output,
                   const std::array<std::array<double, 56>, SIZECELLS>& values){
    for(std::size_t i = 0; i < SIZE; ++i){
        auto table = values.at(type.at(i));
        output.at(i) = table.at( index_i.at(i) *8+index_j.at(i));
    }
}

int main(){
    const unsigned int num_elements = 32768;
    const unsigned int num_inputs = 65536;
    const unsigned int num_library_cells = 211;

    //cell library
    std::array<std::array<double, 7>, num_library_cells> slew_index;
    std::array<std::array<double, 8>, num_library_cells> capacitance_index;
    std::array<std::array<double, 56>, num_library_cells> values;

    for(unsigned int i = 0; i < num_library_cells; ++i){
        slew_index.at(i) = {0.8, 2.3, 3.6, 4.1, 6.0, 8.5, 10.0};
        capacitance_index.at(i) = {1.2, 2.7, 3.4, 4.2, 5.6, 7.8, 8.1, 10.0};
        values.at(i) = {1,  2,  3,  4,  5,  6,  7,  8,
                        9, 10, 11, 12, 13, 14, 15, 16,
                       17, 18, 19, 20, 21, 22, 23, 24,
                       25, 26, 27, 28, 29, 30, 31, 32,
                       33, 34, 35, 36, 37, 38, 39, 40,
                       41, 42, 43, 44, 45, 46, 47, 48,
                       49, 50, 51, 52, 53, 54, 55, 56};
    }

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

    //circuit cells
    std::array<double, num_elements> input_slew;
    std::array<double, num_elements> out_capacitance;
    std::array<double, num_elements> delay;
    std::array<double, num_elements> type;
    std::array<double, num_elements> i_index;
    std::array<double, num_elements> j_index;

    //set the circuit's cells type
    for(unsigned int i = 0; i < num_elements;++i){
        type[i] = ports_type.at(i);
    }
    //set cell's input slew and out cap
    unsigned int i, j;
    for(i = 0, j = 0; i < num_elements; ++i, j+=2){
        input_slew.at(i) = lut_input.at(j);
        out_capacitance.at(i) = lut_input.at(j+1);
    }

    std::clock_t begin = clock();

    //find slew indexes
    find_indexes(input_slew, type, i_index, slew_index);

    //find cap indexes
    find_indexes(out_capacitance, type, j_index, capacitance_index);

    //do interpolation
    interpolation(i_index, j_index, type, delay, values);

    std::clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    std::cout<<"elapsed: "<<elapsed_secs<<std::endl;

    return 0;
}
