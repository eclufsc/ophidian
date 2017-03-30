#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include <iostream>
#include <memory>

#include "./experiment.h"
#include "./metric.h"
#include "./problem_a.h"
#include "./problem_b.h"

using namespace ophidian;

TEST_CASE("Test all circuits runtime DOD Problem A", "[problem_a][runtime][DOD]"){
    std::vector<std::string> circuits = {"simple", "superblue18", "superblue4", "superblue16", "superblue5", "superblue1", "superblue3", "superblue10", "superblue7"};
    for(auto circuit_name : circuits){
        Experiment exp(circuit_name);
        std::unique_ptr<Runtime> runtime = std::unique_ptr<Runtime>(new Runtime());
        problem_a::problem_a_dod(exp.def.get(), exp.lef.get(), *exp.m_placement, *runtime);
    }
}

TEST_CASE("Test all circuits runtime OOD Problem A", "[problem_a][runtime][OOD]"){
    std::vector<std::string> circuits = {"simple", "superblue18", "superblue4", "superblue16", "superblue5", "superblue1", "superblue3", "superblue10", "superblue7"};
    for(auto circuit_name : circuits){
        Experiment exp(circuit_name);
        std::unique_ptr<Runtime> runtime = std::unique_ptr<Runtime>(new Runtime());
        problem_a::problem_a_ood(exp.def.get(), exp.lef.get(), *exp.m_placement, *runtime);
    }
}

TEST_CASE("Test all circuits runtime DOD Problem B", "[problem_b][runtime][DOD]"){
    std::vector<std::string> circuits = {"simple", "superblue18", "superblue4", "superblue16", "superblue5", "superblue1", "superblue3", "superblue10", "superblue7"};
    for(auto circuit_name : circuits){
        Experiment exp(circuit_name);
        std::unique_ptr<Runtime> runtime = std::unique_ptr<Runtime>(new Runtime());
        problem_b::problem_b_dod(*exp.m_placement, *runtime);
    }
}

TEST_CASE("Test all circuits runtime OOD Problem B", "[problem_b][runtime][OOD]"){
    std::vector<std::string> circuits = {"simple", "superblue18", "superblue4", "superblue16", "superblue5", "superblue1", "superblue3", "superblue10", "superblue7"};
    for(auto circuit_name : circuits){
        Experiment exp(circuit_name);
        std::unique_ptr<Runtime> runtime = std::unique_ptr<Runtime>(new Runtime());
        problem_b::problem_b_ood(*exp.m_placement, *runtime);
    }
}

TEST_CASE("Test all circuits miss DOD Problem A", "[problem_a][miss][DOD]"){
    std::vector<std::string> circuits = {"simple", "superblue18", "superblue4", "superblue16", "superblue5", "superblue1", "superblue3", "superblue10", "superblue7"};
    for(auto circuit_name : circuits){
        Experiment exp(circuit_name);
        int PAPI_events[] = {PAPI_L1_ICM, PAPI_L1_DCM};//Please change this according with your cpu architecture.
        std::unique_ptr<Miss> miss = std::unique_ptr<Miss>(new Miss(PAPI_events, 2));
        problem_a::problem_a_dod(exp.def.get(), exp.lef.get(), *exp.m_placement, *miss);
    }
}

TEST_CASE("Test all circuits miss OOD Problem A", "[problem_a][miss][OOD]"){
    std::vector<std::string> circuits = {"simple", "superblue18", "superblue4", "superblue16", "superblue5", "superblue1", "superblue3", "superblue10", "superblue7"};
    for(auto circuit_name : circuits){
        Experiment exp(circuit_name);
        int PAPI_events[] = {PAPI_L1_ICM, PAPI_L1_DCM};//Please change this according with your cpu architecture.
        std::unique_ptr<Miss> miss = std::unique_ptr<Miss>(new Miss(PAPI_events, 2));
        problem_a::problem_a_ood(exp.def.get(), exp.lef.get(), *exp.m_placement, *miss);
    }
}

TEST_CASE("Test all circuits miss DOD Problem B", "[problem_b][miss][DOD]"){
    std::vector<std::string> circuits = {"simple", "superblue18", "superblue4", "superblue16", "superblue5", "superblue1", "superblue3", "superblue10", "superblue7"};
    for(auto circuit_name : circuits){
        Experiment exp(circuit_name);
        int PAPI_events[] = {PAPI_L1_ICM, PAPI_L1_DCM};//Please change this according with your cpu architecture.
        std::unique_ptr<Miss> miss = std::unique_ptr<Miss>(new Miss(PAPI_events, 2));
        problem_b::problem_b_dod(*exp.m_placement, *miss);
    }
}

TEST_CASE("Test all circuits miss OOD Problem B", "[problem_b][miss][OOD]"){
    std::vector<std::string> circuits = {"simple", "superblue18", "superblue4", "superblue16", "superblue5", "superblue1", "superblue3", "superblue10", "superblue7"};
    for(auto circuit_name : circuits){
        Experiment exp(circuit_name);
        int PAPI_events[] = {PAPI_L1_ICM, PAPI_L1_DCM};//Please change this according with your cpu architecture.
        std::unique_ptr<Miss> miss = std::unique_ptr<Miss>(new Miss(PAPI_events, 2));
        problem_b::problem_b_ood(*exp.m_placement, *miss);
    }
}
