
#include "date21.h"
#include <iostream>
#include <fstream>
#include <ophidian/util/log.h>
#include <ophidian/routing/Engine.h>
#include <ophidian/routing/MCFMultiThreading.h>
#include <ophidian/routing/RoutabilityCheck.h>

using namespace ophidian::util;
using point_type = ophidian::util::LocationDbu;
using cell_type = ophidian::circuit::Netlist::cell_instance_type;
// using movements_type = std::vector<std::pair<ophidian::circuit::Netlist::cell_instance_type, ophidian::util::LocationDbu>>;
using movements_type = std::unordered_map< ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type, ophidian::entity_system::EntityBaseHash>; 



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

    ophidian::routing::check_connectivity(design, nets);
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

    ophidian::routing::check_connectivity(design, nets);
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

    ophidian::routing::check_connectivity(design, nets);
}

void draw_gcell_svg(ophidian::design::Design & design, std::string net_name){
    auto& netlist = design.netlist();
    auto& routing_library = design.routing_library();
    auto& global_routing = design.global_routing();
    auto gcell_graph_ptr = global_routing.gcell_graph();
    std::ofstream out_svg;
    if (net_name == "")
        out_svg.open("output.svg");
    else
        out_svg.open(net_name + ".svg");

    //layer index
    std::unordered_map<int, std::string> layer2color = {
        {1,"#0000ff"},//blue
        {2,"#ff0000"},//red
        {3,"#00d000"},//green
        {4,"#d0d000"},//yellow
        {5,"#a52a2a"},//dark red
        {6,"#ffa500"},//orange
        {7,"#d000d0"},//pink
        {8,"#00d0d0"},//light blue
        {9,"#a52a2a"},//brown
        {10,"#ffff00"},//light yellow
        {11,"#008000"},//dark green
        {12,"#ff00ff"},//purple
        {13,"#ffc0cb"},//light pink
        {14,"#00ffff"},//light blue
        {15,"#800080"},//dark purple
        {16,"#808000"},//dark yellow
    };

    out_svg<<"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
    out_svg<<"<svg>\n";

    for(auto layer_color : layer2color)
    {
        out_svg<<"<g\n";
        out_svg<<"inkscape:label=\""<<layer_color.first<<"\"\n";
        out_svg<<"inkscape:groupmode=\"layer\"\n";
        out_svg<<"id=\""<<layer_color.first<<"\">\n";
        for(auto net_it = netlist.begin_net(); net_it != netlist.end_net(); ++net_it)
        {
            if(net_name != "" && netlist.name(*net_it) != net_name)
                continue;

            for(auto pin : netlist.pins(*net_it))
            {
                for(auto geometry : design.placement().geometry(pin))
                {
                    auto layer = routing_library.find_layer_instance(geometry.second);
                    if(routing_library.layerIndex(layer) != layer_color.first)
                        continue;
                    auto box = geometry.first;
                    auto width = box.max_corner().x() - box.min_corner().x();
                    auto height = box.max_corner().y() - box.min_corner().y();

                    out_svg<<"<rect\n";
                    out_svg<<"style=\"fill:"<< "#C0C0C0" <<";fill-opacity:0.5;\"\n"; // #C0C0C0 -> HTML Silver
                    out_svg<<"width=\""<<units::unit_cast<double>(width) / 1000<<"\"\n";
                    out_svg<<"height=\""<<units::unit_cast<double>(height) / 1000<<"\"\n";
                    out_svg<<"x=\""<<units::unit_cast<double>(box.min_corner().x()) / 1000<<"\"\n";
                    out_svg<<"y=\""<<units::unit_cast<double>(-box.max_corner().y()) / 1000<<"\" />\n";//svg files use y axis flipped
                }
            }

            for(auto gcell : global_routing.gcells(*net_it))
            {
                if(gcell_graph_ptr->layer_index(gcell) != layer_color.first)
                    continue;

                auto box = gcell_graph_ptr->box(gcell);
                auto width = box.max_corner().x() - box.min_corner().x();
                auto height = box.max_corner().y() - box.min_corner().y();

                out_svg<<"<rect\n";
                out_svg<<"style=\"fill:"<<layer_color.second<<";fill-opacity:0.5;\"\n";
                out_svg<<"width=\""<<units::unit_cast<double>(width) / 1000<<"\"\n";
                out_svg<<"height=\""<<units::unit_cast<double>(height) / 1000<<"\"\n";
                out_svg<<"x=\""<<units::unit_cast<double>(box.min_corner().x()) / 1000<<"\"\n";
                out_svg<<"y=\""<<units::unit_cast<double>(-box.max_corner().y()) / 1000<<"\" />\n";//svg files use y axis flipped
            }
        }
        out_svg<<"</g>\n";
    }
    out_svg<<"</svg>";
    out_svg.close();
}
