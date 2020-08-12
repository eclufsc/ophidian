#include <sys/time.h>

#include <catch.hpp>
#include <ophidian/design/DesignFactory.h>
#include <ophidian/parser/ICCAD2020Writer.h>
#include <ophidian/util/log.h>
#include "astar_colapse_net.h"

using namespace ophidian::util;


TEST_CASE("iccad20 AStarRouting on all nets and colapsing nets", "[astar_colapse_nets]")
{
    std::vector<std::string> circuit_names = {
        // "case1",
        // "case2",
        // "case3",
        "case3_no_extra_demand",
        // "case4",
        // "case5",
    };

    std::string benchmarks_path = "./input_files/iccad2020/cases/";
    // std::string benchmarks_path = "./input_files/iccad20/"; //Tiago
    // std::string benchmarks_path = "./benchmarks/"; //Tesla
    for (auto circuit_name : circuit_names) 
    {
        using scalar_type = int;
        using AStarSegment = ophidian::routing::AStarSegment;
        using net_type = ophidian::circuit::Net;
        using cell_type = ophidian::circuit::CellInstance;
        using point_type = ophidian::util::LocationDbu;
        std::string iccad_2020_file = benchmarks_path + circuit_name + ".txt";
        auto iccad_2020 = ophidian::parser::ICCAD2020{iccad_2020_file};
        auto design = ophidian::design::Design();
        ophidian::design::factory::make_design_iccad2020(design, iccad_2020);
        ophidian::parser::ICCAD2020Writer iccad_output_writer(design, circuit_name);
        auto & global_routing = design.global_routing();
        auto & netlist = design.netlist();
        auto & placement = design.placement();

        std::vector<ophidian::circuit::Net> nets(netlist.begin_net(), netlist.end_net());
        int initial_wirelength = global_routing.wirelength(nets);
        log() << "Circuit initial wirelength = " << initial_wirelength << std::endl;

        int routed_nets = 0;
        int non_routed = 0;
        auto astar_routing = ophidian::routing::AStarRouting(design);

        printlog("Initial A* routing ...");
        for(auto net_it = netlist.begin_net(); net_it != netlist.end_net(); net_it++)
        {
            auto net = *net_it;
            std::vector<AStarSegment> initial_segments;
            for(auto segment : global_routing.segments(net))
                initial_segments.push_back(AStarSegment(global_routing.box(segment), global_routing.layer_start(segment), global_routing.layer_end(segment), net));

            global_routing.unroute(net);
            std::vector<AStarSegment> segments;
            auto result = astar_routing.route_net(net, segments, false);
            if(result)
            {
                routed_nets++;
                astar_routing.apply_segments_to_global_routing(segments);
            }else{
                non_routed++;
                astar_routing.apply_segments_to_global_routing(initial_segments);
            }
        }

        int a_wirelength = global_routing.wirelength(nets);
        log() << "Circuit A* wirelength = " << a_wirelength << std::endl;
        auto score = initial_wirelength - a_wirelength;
        log() << "Estimated score ( "<< initial_wirelength << " - " << a_wirelength << " ) = " << score << std::endl;
        double reduction = 1.0 - ( (double) a_wirelength / (double) initial_wirelength) ;
        log() << "% Reduction = " << std::to_string(reduction * 100) << " %" << std::endl;


        std::vector<std::pair<cell_type, point_type>>  movements;

        AstarColapseNet astar_colapse_net(design);
        astar_colapse_net.colapse_nets(nets, movements);


        int colapse_wirelength = global_routing.wirelength(nets);
        log() << "Circuit colapsing wirelength = " << colapse_wirelength << std::endl;
        int partial_score = a_wirelength - colapse_wirelength;
        score = initial_wirelength - colapse_wirelength;
        log() << "Partial score ( "<< a_wirelength << " - " << colapse_wirelength << " ) = " << partial_score << std::endl;
        
        

        printlog("Writing solution ...");
        iccad_output_writer.write_ICCAD_2020_output(circuit_name + "_out.txt", movements);

        log()<<"routed "<<routed_nets<<" of "<<netlist.size_net()<<" non routed "<<non_routed<<std::endl;
        int final_wirelength = global_routing.wirelength(nets);
        log() << "Circuit final wirelength = " << final_wirelength << std::endl;
        score = initial_wirelength - final_wirelength;
        log() << "Estimated score ( "<< initial_wirelength << " - " << final_wirelength << " ) = " << score << std::endl;
        reduction = 1.0 - ( (double) final_wirelength / (double) initial_wirelength) ;
        log() << "% Reduction = " << std::to_string(reduction * 100) << " %" << std::endl;
    }
}
