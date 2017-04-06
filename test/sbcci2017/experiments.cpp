#include "../catch.hpp"
#include "kmeans_data_oriented_design.h"
#include "kmeans_object_oriented_design.h"
#include "kmeans_hybrid.h"
#include "metrics.h"
#include "../../ophidian/parser/Def.h"

#include <memory>

std::vector<std::string> circuits = {
    "superblue18",
    "superblue4",
    "superblue16",
    "superblue5",
    "superblue1",
    "superblue3",
    "superblue10",
    "superblue7"
};

TEST_CASE("kmeans/ run DOD sequential in ICCAD2015","[experiment][kmeans][DOD][sequential][Nrtree]") {
    for(auto circuit_name : circuits){
        ophidian::parser::DefParser reader;
        std::shared_ptr<ophidian::parser::Def> def = reader.readFile("./benchmarks/"+circuit_name+"/"+circuit_name+".def");

        std::vector<ophidian::geometry::Point> flip_flop_positions;

        std::string ff ("DFF_X80");
        for(ophidian::parser::Def::component component : def->components()){
            if(ff.compare(component.macro) == 0){
                flip_flop_positions.push_back(ophidian::geometry::Point(component.position.x, component.position.y));
            }
        }

        auto die = (*def).die();
        ophidian::geometry::Point chipOrigin((double)die.lower.x, (double)die.lower.y);
        ophidian::geometry::Point chipBondary((double)die.upper.x, (double)die.upper.y);


        ophidian::KmeansDataOrientedDesign kmeansDOD (chipOrigin, chipBondary, (int)(flip_flop_positions.size()/50) );

        std::cout<<circuit_name<<" ";
        std::unique_ptr<Runtime> runtime = std::unique_ptr<Runtime>(new Runtime());
        int PAPI_events[] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM};//Please change this according with your cpu architecture.
        std::unique_ptr<Miss> miss = std::unique_ptr<Miss>(new Miss(PAPI_events, 3));
        runtime->start();
        miss->start();
        kmeansDOD.cluster_registers(flip_flop_positions, 10);
        miss->end();
        runtime->end();
        runtime->print_result();
        miss->print_result();


    }


}

TEST_CASE("kmeans/ run OOD sequential in ICCAD2015","[experiment][kmeans][OOD][sequential][Nrtree]") {
    for(auto circuit_name : circuits){
        ophidian::parser::DefParser reader;
        std::shared_ptr<ophidian::parser::Def> def = reader.readFile("./benchmarks/"+circuit_name+"/"+circuit_name+".def");

        std::vector<ophidian::geometry::Point> flip_flop_positions;

        std::string ff ("DFF_X80");
        for(ophidian::parser::Def::component component : def->components()){
            if(ff.compare(component.macro) == 0){
                flip_flop_positions.push_back(ophidian::geometry::Point(component.position.x, component.position.y));
            }
        }

        auto die = (*def).die();
        ophidian::geometry::Point chipOrigin((double)die.lower.x, (double)die.lower.y);
        ophidian::geometry::Point chipBondary((double)die.upper.x, (double)die.upper.y);

        ophidian::KmeansObjectOrientedDesign kmeansOOD (chipOrigin, chipBondary, (int)(flip_flop_positions.size()/50) );

        std::vector<ophidian::FlipFlop> flip_flops;
        flip_flops.reserve(flip_flop_positions.size());
        for(auto p : flip_flop_positions){
            flip_flops.push_back(ophidian::FlipFlop(p));
        }

        std::cout<<circuit_name<<" ";
        std::unique_ptr<Runtime> runtime = std::unique_ptr<Runtime>(new Runtime());
        int PAPI_events[] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM};//Please change this according with your cpu architecture.
        std::unique_ptr<Miss> miss = std::unique_ptr<Miss>(new Miss(PAPI_events, 3));
        runtime->start();
        miss->start();
        kmeansOOD.cluster_registers(flip_flops, 10);
        miss->end();
        runtime->end();
        runtime->print_result();
        miss->print_result();

    }

}



TEST_CASE("kmeans/ run DOD parallel in ICCAD2015","[experiment][kmeans][DOD][parallel][Nrtree]") {
    for(auto circuit_name : circuits){
        ophidian::parser::DefParser reader;
        std::shared_ptr<ophidian::parser::Def> def = reader.readFile("./benchmarks/"+circuit_name+"/"+circuit_name+".def");

        std::vector<ophidian::geometry::Point> flip_flop_positions;

        std::string ff ("DFF_X80");
        for(ophidian::parser::Def::component component : def->components()){
            if(ff.compare(component.macro) == 0){
                flip_flop_positions.push_back(ophidian::geometry::Point(component.position.x, component.position.y));
            }
        }

        auto die = (*def).die();
        ophidian::geometry::Point chipOrigin((double)die.lower.x, (double)die.lower.y);
        ophidian::geometry::Point chipBondary((double)die.upper.x, (double)die.upper.y);


        ophidian::KmeansDataOrientedDesign kmeansDOD (chipOrigin, chipBondary, (int)(flip_flop_positions.size()/50) );


        std::cout<<circuit_name<<" ";
        std::unique_ptr<Runtime> runtime = std::unique_ptr<Runtime>(new Runtime());
        int PAPI_events[] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM};//Please change this according with your cpu architecture.
        std::unique_ptr<Miss> miss = std::unique_ptr<Miss>(new Miss(PAPI_events, 3));
        runtime->start();
        miss->start();
        kmeansDOD.cluster_registers_parallel(flip_flop_positions, 10);
        miss->end();
        runtime->end();
        runtime->print_result();
        miss->print_result();


    }


}

TEST_CASE("kmeans/ run OOD parallel in ICCAD2015","[experiment][kmeans][OOD][parallel][Nrtree]") {
    for(auto circuit_name : circuits){
        ophidian::parser::DefParser reader;
        std::shared_ptr<ophidian::parser::Def> def = reader.readFile("./benchmarks/"+circuit_name+"/"+circuit_name+".def");

        std::vector<ophidian::geometry::Point> flip_flop_positions;

        std::string ff ("DFF_X80");
        for(ophidian::parser::Def::component component : def->components()){
            if(ff.compare(component.macro) == 0){
                flip_flop_positions.push_back(ophidian::geometry::Point(component.position.x, component.position.y));
            }
        }

        auto die = (*def).die();
        ophidian::geometry::Point chipOrigin((double)die.lower.x, (double)die.lower.y);
        ophidian::geometry::Point chipBondary((double)die.upper.x, (double)die.upper.y);

        ophidian::KmeansObjectOrientedDesign kmeansOOD (chipOrigin, chipBondary, (int)(flip_flop_positions.size()/50) );

        std::vector<ophidian::FlipFlop> flip_flops;
        flip_flops.reserve(flip_flop_positions.size());
        for(auto p : flip_flop_positions){
            flip_flops.push_back(ophidian::FlipFlop(p));
        }


        std::cout<<circuit_name<<" ";
        std::unique_ptr<Runtime> runtime = std::unique_ptr<Runtime>(new Runtime());
        int PAPI_events[] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM};//Please change this according with your cpu architecture.
        std::unique_ptr<Miss> miss = std::unique_ptr<Miss>(new Miss(PAPI_events, 3));
        runtime->start();
        miss->start();
        kmeansOOD.cluster_registers_paralel(flip_flops, 10);
        miss->end();
        runtime->end();
        runtime->print_result();
        miss->print_result();

    }

}



//--WITH RTree
TEST_CASE("kmeans/ run DOD sequential with rtree in ICCAD2015","[experiment][kmeans][DOD][sequential][rtree]") {
    for(auto circuit_name : circuits){
        ophidian::parser::DefParser reader;
        std::shared_ptr<ophidian::parser::Def> def = reader.readFile("./benchmarks/"+circuit_name+"/"+circuit_name+".def");

        std::vector<ophidian::geometry::Point> flip_flop_positions;

        std::string ff ("DFF_X80");
        for(ophidian::parser::Def::component component : def->components()){
            if(ff.compare(component.macro) == 0){
                flip_flop_positions.push_back(ophidian::geometry::Point(component.position.x, component.position.y));
            }
        }

        auto die = (*def).die();
        ophidian::geometry::Point chipOrigin((double)die.lower.x, (double)die.lower.y);
        ophidian::geometry::Point chipBondary((double)die.upper.x, (double)die.upper.y);


        ophidian::KmeansDataOrientedDesign kmeansDOD (chipOrigin, chipBondary, (int)(flip_flop_positions.size()/50) );

        std::cout<<circuit_name<<" ";
        std::unique_ptr<Runtime> runtime = std::unique_ptr<Runtime>(new Runtime());
        int PAPI_events[] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM};//Please change this according with your cpu architecture.
        std::unique_ptr<Miss> miss = std::unique_ptr<Miss>(new Miss(PAPI_events, 3));
        runtime->start();
        miss->start();
        kmeansDOD.cluster_registers_with_rtree(flip_flop_positions, 10);
        miss->end();
        runtime->end();
        runtime->print_result();
        miss->print_result();


    }


}

TEST_CASE("kmeans/ run OOD sequential with rtree in ICCAD2015","[experiment][kmeans][OOD][sequential][rtree]") {
    for(auto circuit_name : circuits){
        ophidian::parser::DefParser reader;
        std::shared_ptr<ophidian::parser::Def> def = reader.readFile("./benchmarks/"+circuit_name+"/"+circuit_name+".def");

        std::vector<ophidian::geometry::Point> flip_flop_positions;

        std::string ff ("DFF_X80");
        for(ophidian::parser::Def::component component : def->components()){
            if(ff.compare(component.macro) == 0){
                flip_flop_positions.push_back(ophidian::geometry::Point(component.position.x, component.position.y));
            }
        }

        auto die = (*def).die();
        ophidian::geometry::Point chipOrigin((double)die.lower.x, (double)die.lower.y);
        ophidian::geometry::Point chipBondary((double)die.upper.x, (double)die.upper.y);

        ophidian::KmeansObjectOrientedDesign kmeansOOD (chipOrigin, chipBondary, (int)(flip_flop_positions.size()/50) );

        std::vector<ophidian::FlipFlop> flip_flops;
        flip_flops.reserve(flip_flop_positions.size());
        for(auto p : flip_flop_positions){
            flip_flops.push_back(ophidian::FlipFlop(p));
        }

        std::cout<<circuit_name<<" ";
        std::unique_ptr<Runtime> runtime = std::unique_ptr<Runtime>(new Runtime());
        int PAPI_events[] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM};//Please change this according with your cpu architecture.
        std::unique_ptr<Miss> miss = std::unique_ptr<Miss>(new Miss(PAPI_events, 3));
        runtime->start();
        miss->start();
        kmeansOOD.cluster_registers_with_rtree(flip_flops, 10);
        miss->end();
        runtime->end();
        runtime->print_result();
        miss->print_result();

    }

}



TEST_CASE("kmeans/ run DOD parallel with rtree in ICCAD2015","[experiment][kmeans][DOD][parallel][rtree]") {
    for(auto circuit_name : circuits){
        ophidian::parser::DefParser reader;
        std::shared_ptr<ophidian::parser::Def> def = reader.readFile("./benchmarks/"+circuit_name+"/"+circuit_name+".def");

        std::vector<ophidian::geometry::Point> flip_flop_positions;

        std::string ff ("DFF_X80");
        for(ophidian::parser::Def::component component : def->components()){
            if(ff.compare(component.macro) == 0){
                flip_flop_positions.push_back(ophidian::geometry::Point(component.position.x, component.position.y));
            }
        }

        auto die = (*def).die();
        ophidian::geometry::Point chipOrigin((double)die.lower.x, (double)die.lower.y);
        ophidian::geometry::Point chipBondary((double)die.upper.x, (double)die.upper.y);


        ophidian::KmeansDataOrientedDesign kmeansDOD (chipOrigin, chipBondary, (int)(flip_flop_positions.size()/50) );


        std::cout<<circuit_name<<" ";
        std::unique_ptr<Runtime> runtime = std::unique_ptr<Runtime>(new Runtime());
        int PAPI_events[] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM};//Please change this according with your cpu architecture.
        std::unique_ptr<Miss> miss = std::unique_ptr<Miss>(new Miss(PAPI_events, 3));
        runtime->start();
        miss->start();
        kmeansDOD.cluster_registers_with_rtree_parallel(flip_flop_positions, 10);
        miss->end();
        runtime->end();
        runtime->print_result();
        miss->print_result();


    }


}

TEST_CASE("kmeans/ run OOD parallel with rtree in ICCAD2015","[experiment][kmeans][OOD][parallel][rtree]") {
    for(auto circuit_name : circuits){
        ophidian::parser::DefParser reader;
        std::shared_ptr<ophidian::parser::Def> def = reader.readFile("./benchmarks/"+circuit_name+"/"+circuit_name+".def");

        std::vector<ophidian::geometry::Point> flip_flop_positions;

        std::string ff ("DFF_X80");
        for(ophidian::parser::Def::component component : def->components()){
            if(ff.compare(component.macro) == 0){
                flip_flop_positions.push_back(ophidian::geometry::Point(component.position.x, component.position.y));
            }
        }

        auto die = (*def).die();
        ophidian::geometry::Point chipOrigin((double)die.lower.x, (double)die.lower.y);
        ophidian::geometry::Point chipBondary((double)die.upper.x, (double)die.upper.y);

        ophidian::KmeansObjectOrientedDesign kmeansOOD (chipOrigin, chipBondary, (int)(flip_flop_positions.size()/50) );

        std::vector<ophidian::FlipFlop> flip_flops;
        flip_flops.reserve(flip_flop_positions.size());
        for(auto p : flip_flop_positions){
            flip_flops.push_back(ophidian::FlipFlop(p));
        }


        std::cout<<circuit_name<<" ";
        std::unique_ptr<Runtime> runtime = std::unique_ptr<Runtime>(new Runtime());
        int PAPI_events[] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM};//Please change this according with your cpu architecture.
        std::unique_ptr<Miss> miss = std::unique_ptr<Miss>(new Miss(PAPI_events, 3));
        runtime->start();
        miss->start();
        kmeansOOD.cluster_registers_with_rtree_paralel(flip_flops, 10);
        miss->end();
        runtime->end();
        runtime->print_result();
        miss->print_result();

    }

}

