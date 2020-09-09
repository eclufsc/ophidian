
#include "date21.h"
#include <iostream>
#include <fstream>

void write_csv_header(std::string csv_file_name) {
    std::ofstream csv_file(csv_file_name, std::ofstream::out);

    csv_file << "design,wirelength,number of vias,runtime" << std::endl;
}

void write_csv(ophidian::design::Design & design, std::string circuit_name, std::string csv_file_name, double runtime) {
    std::ofstream csv_file(csv_file_name, std::ofstream::app);

    auto nets = std::vector<ophidian::circuit::Net>{design.netlist().begin_net(), design.netlist().end_net()};

    auto wirelength = design.global_routing().wirelength(nets);
    auto vias = design.global_routing().number_of_vias(nets);

    csv_file << circuit_name << "," << wirelength << "," << vias << "," << runtime << std::endl;
}

void Astar_without_paneling_and_without_movements(ophidian::design::Design & design, std::string circuit_name, std::string output)
{

}

void Astar_with_paneling_and_without_movements(ophidian::design::Design & design, std::string circuit_name, std::string output)
{

}

void Astar_with_paneling_and_with_movements(ophidian::design::Design & design, std::string circuit_name, std::string output)
{

}

void Astar_with_paneling_and_with_movements_parallel(ophidian::design::Design & design, std::string circuit_name, std::string output)
{

}

void ILP_without_movements_Astar_without_movements(ophidian::design::Design & design, std::string circuit_name, std::string output)
{

}

void ILP_with_movements_Astar_with_movements(ophidian::design::Design & design, std::string circuit_name, std::string output)
{

}

void ILP_with_movements_Astar_with_movements_parallel(ophidian::design::Design & design, std::string circuit_name, std::string output)
{

}
