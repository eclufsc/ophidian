#include "../catch.hpp"
#include "kmeans_data_oriented_design.h"
#include "kmeans_object_oriented_design.h"

#include "../../ophidian/parser/Def.h"
#include <chrono>

TEST_CASE("kmeans/ initialize random clusters","[kmeansDOD]") {
    ophidian::geometry::Point chipOrigin(1.0, 1.0);
    ophidian::geometry::Point chipBondary(100.0, 100.0);

    ophidian::KmeansDataOrientedDesign kmeansDOD (chipOrigin, chipBondary, 25);
    REQUIRE(kmeansDOD.clusters_.size() == 25);
    REQUIRE(kmeansDOD.clusterCenters_.size() == 25);

    for(auto p : kmeansDOD.clusterCenters_){
        REQUIRE(p.x() >= chipOrigin.x());
        REQUIRE(p.y() >= chipOrigin.y());
        REQUIRE(p.x() <= chipBondary.x());
        REQUIRE(p.y() <= chipBondary.y());
    }
}

bool point_comparison(const ophidian::geometry::Point & point1, const ophidian::geometry::Point & point2){
    return boost::geometry::equals(point1, point2);
}

bool cluster_comparison(const std::vector<ophidian::geometry::Point> & cluster1, const std::vector<ophidian::geometry::Point> & cluster2) {
    if (cluster1.size() != cluster2.size()) {
        return false;
    }
    return std::is_permutation(cluster1.begin(), cluster1.end(), cluster2.begin(), point_comparison);
}

TEST_CASE("kmeans/ initialize clusters by vector","[kmeansDOD]") {
    std::vector<ophidian::geometry::Point> flip_flop_positions = {
       {1, 1},
       {3, 2},
       {2, 1},
       {1, 3},
       {7, 2},
       {7, 1},
       {9, 1},
       {9, 3},
       {1, 9},
       {3, 7},
       {1, 8},
       {2, 7},
       {7, 7},
       {7, 9},
       {7, 8},
       {8, 7}
   };
   std::vector<ophidian::geometry::Point> initial_centers = {
        {2, 2},
        {8, 2},
        {2, 8},
        {8, 8}
   };
   std::vector<ophidian::geometry::Point> expected_centers = {
        {1.75, 1.75},
        {8, 1.75},
        {1.75, 7.75},
        {7.25, 7.75}
   };
   std::vector<std::vector<ophidian::geometry::Point>> expected_clusters = {
        {{1, 1}, {3, 2}, {2, 1}, {1, 3}},
        {{7, 2}, {7, 1}, {9, 1}, {9, 3}},
        {{1, 9}, {3, 7}, {1, 8}, {2, 7}},
        {{7, 7}, {7, 9}, {7, 8}, {8, 7}}
   };

   ophidian::KmeansDataOrientedDesign kmeansDOD(initial_centers);
   REQUIRE(kmeansDOD.clusters_.size() == initial_centers.size());

   kmeansDOD.cluster_registers(flip_flop_positions, 1);

   REQUIRE(kmeansDOD.clusterCenters_.size() == expected_centers.size());
   REQUIRE(std::is_permutation(kmeansDOD.clusterCenters_.begin(), kmeansDOD.clusterCenters_.end(), expected_centers.begin(), point_comparison));

   REQUIRE(kmeansDOD.clusterElements_.size() == expected_clusters.size());
   REQUIRE(std::is_permutation(kmeansDOD.clusterElements_.begin(), kmeansDOD.clusterElements_.end(), expected_clusters.begin(), cluster_comparison));

}




TEST_CASE("kmeans/ run DOD non paralel in ICCAD2015","[kmeansDODnonParalel]") {
    std::vector<std::string> circuits = {
        "superblue18",
//        "superblue4",
//        "superblue16",
//        "superblue5",
//        "superblue1",
//        "superblue3",
//        "superblue10",
//        "superblue7"
    };




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

        auto time_start = std::chrono::high_resolution_clock::now();
        kmeansDOD.cluster_registers(flip_flop_positions, 10);
        auto time_end = std::chrono::high_resolution_clock::now();

        auto total_time = time_end - time_start;
        std::cout <<"Runtime kmeans: " << std::chrono::duration_cast<std::chrono::milliseconds>(total_time).count()<<" ms"<<std::endl;

    }


}




TEST_CASE("kmeans/ basic test structure","[kmeansOOD]") {
   using Point = ophidian::geometry::Point;

   Point a(5,7);
   ophidian::ClusterOOD cluster(a);
   REQUIRE( cluster.clusterCenter().x() == 5 );
   REQUIRE( cluster.clusterCenter().y() == 7 );

   cluster.setClusterCenter(Point(8,9));
   REQUIRE( cluster.clusterCenter().x() == 8 );
   REQUIRE( cluster.clusterCenter().y() == 9 );

   Point ff(1,2);
   ophidian::FlipFlop element(ff);
   REQUIRE( element.position().x() == 1);
   REQUIRE( element.position().y() == 2);
   element.setPosition(Point(3,4));
   REQUIRE( element.position().x() == 3);
   REQUIRE( element.position().y() == 4);

   cluster.insertElement(element);
   REQUIRE(cluster.size() == 1);

   ophidian::FlipFlop ff_2 = cluster.clusterElements().front();
   REQUIRE(ff_2.position().x() == 3);
   REQUIRE(ff_2.position().y()== 4);

}














