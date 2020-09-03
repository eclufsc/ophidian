#include "GuideWriter.h"

namespace ophidian::parser
{

//! writes global routing guides to the specified output path
void write_guide(const ophidian::design::Design& design, const std::string output_path)
{
    std::string path;
    //verify type of file (".guide") in output path
    if(output_path.substr(output_path.find_last_of(".") + 1) == "guide") {
        path = output_path;
    }else{
        path = output_path + ".guide";
    }

    std::ofstream output_file;
    output_file.open(path, std::ios::out|std::ios::trunc);

    auto& global_routing = design.global_routing();
    auto& routing_library = design.routing_library();
    auto& netlist = design.netlist();
    
    output_file << std::fixed << std::setprecision(0);

    for(auto net_it = netlist.begin_net(); net_it != netlist.end_net(); ++net_it)
    {
        // printing net name
        output_file << netlist.name(*net_it) << std::endl;
        // open guide container
        output_file << "(" << std::endl;
        // print all guide segments
        for(auto segment : global_routing.segments(*net_it))
        {   
            auto start_layer = global_routing.layer_start(segment);
            auto end_layer = global_routing.layer_end(segment);
            auto start_gcell = global_routing.gcell_start(segment);
            auto end_gcell = global_routing.gcell_end(segment);

            if(start_layer != end_layer){
                //via
                auto min_idx = std::min(routing_library.index(start_layer), routing_library.index(end_layer));
                auto max_idx = std::max(routing_library.index(start_layer), routing_library.index(end_layer));
                auto box = global_routing.gcell_graph()->box(start_gcell);
                
                for(int i = min_idx; i <= max_idx; i++)
                {
                    auto layer_name = routing_library.name( routing_library.layer_from_index(i));
                    //print rebion box
                    output_file << box.min_corner().x().value() << " ";
                    output_file << box.min_corner().y().value() << " ";
                    output_file << box.max_corner().x().value() << " ";
                    output_file << box.max_corner().y().value() << " ";
                    //print region layer
                    output_file << layer_name << std::endl;
                }
            }else{
                //wire
                auto layer_name = routing_library.name(start_layer);

                auto box_start = global_routing.gcell_graph()->box(start_gcell);
                auto box_emd = global_routing.gcell_graph()->box(end_gcell);

                auto min_corner_x = std::min({box_start.min_corner().x().value(), box_start.max_corner().x().value(), box_emd.min_corner().x().value(), box_emd.max_corner().x().value()});
                auto min_corner_y = std::min({box_start.min_corner().y().value(), box_start.max_corner().y().value(), box_emd.min_corner().y().value(), box_emd.max_corner().y().value()});
                auto max_corner_x = std::max({box_start.min_corner().x().value(), box_start.max_corner().x().value(), box_emd.min_corner().x().value(), box_emd.max_corner().x().value()});
                auto max_corner_y = std::max({box_start.min_corner().y().value(), box_start.max_corner().y().value(), box_emd.min_corner().y().value(), box_emd.max_corner().y().value()});
                //print rebion box
                output_file << min_corner_x << " ";
                output_file << min_corner_y << " ";
                output_file << max_corner_x << " ";
                output_file << max_corner_y << " ";
                //print region layer
                output_file << layer_name << std::endl;
            }
        }
        // close guide container
        output_file << ")" << std::endl;
    }

    //closing file
    output_file.close();
}

} // namespace ophidian::parser
