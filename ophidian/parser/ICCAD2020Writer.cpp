#include "ICCAD2020Writer.h"

namespace ophidian::parser
{
    ICCAD2020Writer::ICCAD2020Writer(design_type & design, std::string circuit_name)
        : m_design(design), m_circuit_name(circuit_name)
    {
    }

    void ICCAD2020Writer::write_ICCAD_2020_output(const std::string out_file_name, const std::vector<std::pair<cell_type, point_type>> & movements){
        auto & netlist = m_design.netlist();
        auto & global_routing = m_design.global_routing();
        std::ofstream out_file(out_file_name, std::ofstream::out);
        out_file << "NumMovedCellInst " << movements.size() << std::endl;
        for (auto move : movements) {
            int x = move.second.x().value() / 10 +1;
            int y = move.second.y().value() / 10 +1;
            out_file << "CellInst " << netlist.name(move.first) << " " << y << " " << x << std::endl;
        }

        std::cout << "writing output" << std::endl;
        std::vector<std::string> segment_strings; 
        for(auto net_it = netlist.begin_net(); net_it != netlist.end_net(); net_it++){
            auto net = *net_it;
            auto net_name = netlist.name(net);     

            for(auto segment : global_routing.segments(net)){
                auto box = global_routing.box(segment);
                auto start = box.min_corner();
                auto end = box.max_corner();
                auto start_layer = global_routing.layer_start(segment);
                auto end_layer = global_routing.layer_end(segment);
                auto start_layer_index = m_design.routing_library().layerIndex(start_layer);
                auto end_layer_index = m_design.routing_library().layerIndex(end_layer);

                auto segment_string = std::to_string(((int)start.y().value() - 5) / 10 +1) + " " + std::to_string(((int)start.x().value() - 5) / 10 +1) + " " 
                    + std::to_string(start_layer_index) + " " + std::to_string(((int)end.y().value() - 5) / 10 +1) + " " 
                    + std::to_string(((int)end.x().value() - 5) / 10 +1) + " " + std::to_string(end_layer_index) + " " + net_name;

                segment_strings.push_back(segment_string);
            }
        }

        out_file << "NumRoutes " << segment_strings.size() << std::endl;
        for (auto route_string : segment_strings) {
            out_file << route_string << std::endl;
        }
    }
}
