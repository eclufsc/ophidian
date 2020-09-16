
#include "date21.h"
#include <iostream>
#include <fstream>
#include <ophidian/util/log.h>
#include <ophidian/routing/Engine.h>
#include <ophidian/routing/MCFMultiThreading.h>

using namespace ophidian::util;
using point_type = ophidian::util::LocationDbu;
using cell_type = ophidian::circuit::Netlist::cell_instance_type;
using movements_type = std::vector<std::pair<ophidian::circuit::Netlist::cell_instance_type, ophidian::util::LocationDbu>>;

bool fileExists(std::string fileName)
{
    ifstream infile(fileName);
    return infile.good();
}

void write_csv_header(std::string csv_file_name) {
    if(!fileExists(csv_file_name)){
        std::ofstream csv_file(csv_file_name, std::ofstream::out);
        csv_file << "design,wirelength,number of vias,movements,runtime" << std::endl;
    }
}

void write_csv(ophidian::design::Design & design, std::string circuit_name, std::string csv_file_name, double runtime, unsigned number_of_movements) {
    std::ofstream csv_file(csv_file_name, std::ofstream::app);

    auto nets = std::vector<ophidian::circuit::Net>{design.netlist().begin_net(), design.netlist().end_net()};

    auto wirelength = design.global_routing().wirelength(nets);
    auto vias = design.global_routing().number_of_vias(nets);

    csv_file << circuit_name << "," << wirelength << "," << vias << "," << number_of_movements << "," << runtime << std::endl;
}

bool check_nets_connectivity(const ophidian::design::Design & design, const std::vector<ophidian::circuit::Net>& nets){
    log() << "Checking for disconnected nets..." << std::endl;
    bool is_nets_open = false;
    for (auto net : nets) {
        auto net_name = design.netlist().name(net);
        ophidian::routing::GlobalRouting::gcell_container_type pin_gcells = {};
        for (auto pin : design.netlist().pins(net)) {
            auto pin_name = design.netlist().name(pin);                
            auto location = design.placement().location(pin);
            auto box = ophidian::routing::GCellGraph::box_type{location, location};
            auto pin_geometry = design.placement().geometry(pin);
            auto layer_name = pin_geometry.front().second;
            auto pin_layer = design.routing_library().find_layer_instance(layer_name);
            auto layer_index = design.routing_library().layerIndex(pin_layer);

            // log() << "pin " << pin_name << " layer " << layer_name << " index " << layer_index << std::endl;

            design.global_routing().gcell_graph()->intersect(pin_gcells, box, layer_index-1);
        }
        if(false){ //debug code
            for(auto gcell : pin_gcells)
            {
                auto box = design.global_routing().gcell_graph()->box(gcell);
                auto layer = design.global_routing().gcell_graph()->layer_index(gcell);
                auto layer_i = design.routing_library().layer_from_index(layer);
                auto layer_str = design.routing_library().name(layer_i);
                std::cout << box.min_corner().x().value() << " " << box.min_corner().y().value() << " " << box.max_corner().x().value() << " " << box.max_corner().y().value() << " " << layer_str << std::endl;
            }
        }

        auto connected = design.global_routing().is_connected(net, pin_gcells, net_name);
        
        if(!connected)
        {
            log() << "net " << net_name << " disconnected with " << design.netlist().pins(net).size() << " pins" << std::endl;
            is_nets_open = true;
        }
    }
    if (is_nets_open) printlog("Open nets detected!"); else printlog("All nets connected!");
    return is_nets_open;
}

// "exp1" --> "Astar_without_paneling_and_without_movements"
void Astar_without_paneling_and_without_movements(ophidian::design::Design & design, std::string circuit_name, std::string output)
{

}

// "exp2" --> "Astar_with_paneling_and_without_movements"
void Astar_with_paneling_and_without_movements(ophidian::design::Design & design, std::string circuit_name, std::string output)
{

}

// "exp3" --> "Astar_with_paneling_and_with_movements"
void Astar_with_paneling_and_with_movements(ophidian::design::Design & design, std::string circuit_name, std::string output)
{

}

// "exp4" --> "Astar_with_paneling_and_with_movements_parallel"
void Astar_with_paneling_and_with_movements_parallel(ophidian::design::Design & design, std::string circuit_name, std::string output)
{

}

// "exp5" --> "ILP_without_movements_Astar_without_movements"
void ILP_without_movements_Astar_without_movements(ophidian::design::Design & design, std::string circuit_name, std::string output, movements_type & movements)
{
    printlog("Initing experiment ILP_without_movements_Astar_without_movements");
    design.routing_constraints().set_max_cell_movement(0);

    auto mcf = UCal::MCFMultiThreading(design);
    auto engine = UCal::Engine(design);
    std::vector<ophidian::circuit::Net> nets(design.netlist().begin_net(), design.netlist().end_net());

    design.placement().reset_rtree();
    mcf.construct_net_boxes_rtree(nets);
    mcf.cluster_based_on_panel_v2();
    design.global_routing().set_gcell_cell_instances(design.netlist(), design.placement());
   
    mcf.run_ilp_on_panels_sequential(movements);

    //engine.run_astar_on_panels_sequential(std::numeric_limits<int>::max(), 4);

    check_nets_connectivity(design, nets);
}

// "exp6" --> "ILP_with_movements_Astar_with_movements"
void ILP_with_movements_Astar_with_movements(ophidian::design::Design & design, std::string circuit_name, std::string output, movements_type & movements)
{
    printlog("Initing experiment ILP_with_movements_Astar_with_movements");

    auto mcf = UCal::MCFMultiThreading(design);
    auto engine = UCal::Engine(design);
    std::vector<ophidian::circuit::Net> nets(design.netlist().begin_net(), design.netlist().end_net());

    design.placement().reset_rtree();
    mcf.construct_net_boxes_rtree(nets);
    mcf.cluster_based_on_panel_v2();
    design.global_routing().set_gcell_cell_instances(design.netlist(), design.placement());
    mcf.run_ilp_on_panels_sequential(movements);

    //engine.run_astar_on_panels_sequential(std::numeric_limits<int>::max(), 4);

    check_nets_connectivity(design, nets);
}

// "exp7" --> "ILP_with_movements_Astar_with_movements_parallel"
void ILP_with_movements_Astar_with_movements_parallel(ophidian::design::Design & design, std::string circuit_name, std::string output, movements_type & movements, UCal::Engine & engine)
{
    printlog("Initing ILP_with_movements_Astar_with_movements_parallel");

    //auto mcf = UCal::MCFMultiThreading(design);
    //auto engine = UCal::Engine(design);
    std::vector<ophidian::circuit::Net> nets(design.netlist().begin_net(), design.netlist().end_net());
    
    design.global_routing().set_gcell_cell_instances(design.netlist(), design.placement());

    /*design.placement().reset_rtree();
    mcf.construct_net_boxes_rtree(nets);
    mcf.cluster_based_on_panel_v2();
    mcf.run_ilp_on_panels_parallel(movements);*/

    //engine.run_astar_on_panels_parallel(std::numeric_limits<int>::max(), -1);

    check_nets_connectivity(design, nets);
}
