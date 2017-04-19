#include "../catch.hpp"
#include "kmeans_data_oriented_design.h"
#include "kmeans_object_oriented_design.h"
#include "metrics.h"
#include "../../ophidian/parser/Def.h"

#include <memory>

bool point_comparison(const ophidian::geometry::Point & point1, const ophidian::geometry::Point & point2){
    return boost::geometry::equals(point1, point2);
}

bool cluster_comparison(const std::vector<ophidian::geometry::Point> & cluster1, const std::vector<ophidian::geometry::Point> & cluster2) {
    if (cluster1.size() != cluster2.size()) {
        return false;
    }
    return std::is_permutation(cluster1.begin(), cluster1.end(), cluster2.begin(), point_comparison);
}

TEST_CASE("kmeans/ initialize random clusters","[test][kmeans]") {
    ophidian::geometry::Point chipOrigin(1.0, 1.0);
    ophidian::geometry::Point chipBondary(100.0, 100.0);

    ophidian::KmeansDataOrientedDesign kmeansDOD (chipOrigin, chipBondary, 25);
    REQUIRE(kmeansDOD.clusters_.size() == 25);
    REQUIRE(kmeansDOD.clusterCenters_.size() == 25);

    for(auto p : kmeansDOD.clusterCenters_){
        auto cluster_center = p.first;
        REQUIRE(cluster_center.x() >= chipOrigin.x());
        REQUIRE(cluster_center.y() >= chipOrigin.y());
        REQUIRE(cluster_center.x() <= chipBondary.x());
        REQUIRE(cluster_center.y() <= chipBondary.y());
    }
}

TEST_CASE("kmeans/ basic test structure","[test][kmeans]") {
   using Point = ophidian::geometry::Point;

   Point a(5,7);
   ophidian::ClusterOOD cluster(a);
   REQUIRE( cluster.center().x() == 5 );
   REQUIRE( cluster.center().y() == 7 );

   cluster.center(Point(8,9));
   REQUIRE( cluster.center().x() == 8 );
   REQUIRE( cluster.center().y() == 9 );

   Point ff(1,2);
   ophidian::FlipFlop element(ff);
   REQUIRE( element.position().x() == 1);
   REQUIRE( element.position().y() == 2);
   element.setPosition(Point(3,4));
   REQUIRE( element.position().x() == 3);
   REQUIRE( element.position().y() == 4);

   cluster.insertElement(element);
   REQUIRE(cluster.size() == 1);

   ophidian::FlipFlop ff_2 = cluster.elements().front();
   REQUIRE(ff_2.position().x() == 3);
   REQUIRE(ff_2.position().y()== 4);

}

TEST_CASE("kmeans/ initialize clusters by vector (DOD-sequential)","[test][kmeans]") {
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

   std::vector<ophidian::geometry::Point> clusters_centers;
   clusters_centers.reserve(kmeansDOD.clusterCenters_.size());
   for(auto center_point : kmeansDOD.clusterCenters_)
       clusters_centers.push_back(center_point.first);

   REQUIRE(std::is_permutation(clusters_centers.begin(), clusters_centers.end(), expected_centers.begin(), point_comparison));

   REQUIRE(kmeansDOD.clusterElements_.size() == expected_clusters.size());
   REQUIRE(std::is_permutation(kmeansDOD.clusterElements_.begin(), kmeansDOD.clusterElements_.end(), expected_clusters.begin(), cluster_comparison));

}

TEST_CASE("kmeans/ initialize clusters by vector (DOD-parallel)","[test][kmeans]") {
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

   kmeansDOD.cluster_registers_parallel(flip_flop_positions, 1);

   REQUIRE(kmeansDOD.clusterCenters_.size() == expected_centers.size());

   std::vector<ophidian::geometry::Point> clusters_centers;
   clusters_centers.reserve(kmeansDOD.clusterCenters_.size());
   for(auto center_point : kmeansDOD.clusterCenters_)
       clusters_centers.push_back(center_point.first);
   REQUIRE(std::is_permutation(clusters_centers.begin(), clusters_centers.end(), expected_centers.begin(), point_comparison));

   REQUIRE(kmeansDOD.clusterElements_.size() == expected_clusters.size());
   REQUIRE(std::is_permutation(kmeansDOD.clusterElements_.begin(), kmeansDOD.clusterElements_.end(), expected_clusters.begin(), cluster_comparison));
}

TEST_CASE("kmeans/ initialize clusters by vector (OOD-sequential)","[test][kmeans]") {
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

   ophidian::KmeansObjectOrientedDesign kmeansOOD(initial_centers);
   REQUIRE(kmeansOOD.clusters_.size() == initial_centers.size());

   std::vector<ophidian::FlipFlop> flip_flops;
   flip_flops.reserve(flip_flop_positions.size());
   for(auto p : flip_flop_positions){
       flip_flops.push_back(ophidian::FlipFlop(p));
   }
   kmeansOOD.cluster_registers(flip_flops, 1);

   REQUIRE(kmeansOOD.clusters_.size() == expected_centers.size());
   std::vector<ophidian::geometry::Point> clusters_positions;
   clusters_positions.reserve(kmeansOOD.clusters_.size());
   for(ophidian::ClusterOOD cluster : kmeansOOD.clusters_){
       clusters_positions.push_back(cluster.center());
   }
   REQUIRE(std::is_permutation(clusters_positions.begin(), clusters_positions.end(), expected_centers.begin(), point_comparison));

   REQUIRE(kmeansOOD.clusters_.size() == expected_clusters.size());
   std::vector<std::vector<ophidian::geometry::Point>> cluster_return;
   for(ophidian::ClusterOOD cluster : kmeansOOD.clusters_){
       std::vector<ophidian::geometry::Point> elements_positions;
       elements_positions.reserve(cluster.elements().size());
       for(ophidian::FlipFlop ff : cluster.elements()){
           elements_positions.push_back(ff.position());
       }
       cluster_return.push_back(elements_positions);
   }
   REQUIRE(std::is_permutation(cluster_return.begin(), cluster_return.end(), expected_clusters.begin(), cluster_comparison));
}

TEST_CASE("kmeans/ initialize clusters by vector (OOD-sequential) using Rtree","[test][kmeans]") {
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

   ophidian::KmeansObjectOrientedDesign kmeansOOD(initial_centers);
   REQUIRE(kmeansOOD.clusters_.size() == initial_centers.size());

   std::vector<ophidian::FlipFlop> flip_flops;
   flip_flops.reserve(flip_flop_positions.size());
   for(auto p : flip_flop_positions){
       flip_flops.push_back(ophidian::FlipFlop(p));
   }
   kmeansOOD.cluster_registers_with_rtree_paralel(flip_flops, 1);

   REQUIRE(kmeansOOD.clusters_.size() == expected_centers.size());
   std::vector<ophidian::geometry::Point> clusters_positions;
   clusters_positions.reserve(kmeansOOD.clusters_.size());
   for(ophidian::ClusterOOD cluster : kmeansOOD.clusters_){
       clusters_positions.push_back(cluster.center());
   }
   REQUIRE(std::is_permutation(clusters_positions.begin(), clusters_positions.end(), expected_centers.begin(), point_comparison));

   REQUIRE(kmeansOOD.clusters_.size() == expected_clusters.size());
   std::vector<std::vector<ophidian::geometry::Point>> cluster_return;
   for(ophidian::ClusterOOD cluster : kmeansOOD.clusters_){
       std::vector<ophidian::geometry::Point> elements_positions;
       elements_positions.reserve(cluster.elements().size());
       for(ophidian::FlipFlop ff : cluster.elements()){
           elements_positions.push_back(ff.position());
       }
       cluster_return.push_back(elements_positions);
   }
   REQUIRE(std::is_permutation(cluster_return.begin(), cluster_return.end(), expected_clusters.begin(), cluster_comparison));
}

TEST_CASE("kmeans/ initialize clusters by vector (OOD-parallel)","[test][kmeans]") {
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

   ophidian::KmeansObjectOrientedDesign kmeansOOD(initial_centers);
   REQUIRE(kmeansOOD.clusters_.size() == initial_centers.size());

   std::vector<ophidian::FlipFlop> flip_flops;
   flip_flops.reserve(flip_flop_positions.size());
   for(auto p : flip_flop_positions){
       flip_flops.push_back(ophidian::FlipFlop(p));
   }
   kmeansOOD.cluster_registers_paralel(flip_flops, 1);

   REQUIRE(kmeansOOD.clusters_.size() == expected_centers.size());
   std::vector<ophidian::geometry::Point> clusters_positions;
   clusters_positions.reserve(kmeansOOD.clusters_.size());
   for(ophidian::ClusterOOD cluster : kmeansOOD.clusters_){
       clusters_positions.push_back(cluster.center());
   }
   REQUIRE(std::is_permutation(clusters_positions.begin(), clusters_positions.end(), expected_centers.begin(), point_comparison));

   REQUIRE(kmeansOOD.clusters_.size() == expected_clusters.size());
   std::vector<std::vector<ophidian::geometry::Point>> cluster_return;
   for(ophidian::ClusterOOD cluster : kmeansOOD.clusters_){
       std::vector<ophidian::geometry::Point> elements_positions;
       elements_positions.reserve(cluster.elements().size());
       for(ophidian::FlipFlop ff : cluster.elements()){
           elements_positions.push_back(ff.position());
       }
       cluster_return.push_back(elements_positions);
   }
   REQUIRE(std::is_permutation(cluster_return.begin(), cluster_return.end(), expected_clusters.begin(), cluster_comparison));
}


TEST_CASE("kmeans/ test correctness between implementations of kmeans","[test][kmeans][correctness]") {
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

        int number_of_clusters = (int)(flip_flop_positions.size()/50);
        std::vector<ophidian::geometry::Point> initial_centers;
        initial_centers.reserve(number_of_clusters);

        std::default_random_engine m_generator;
        std::uniform_real_distribution<double> m_distribution_x(chipOrigin.x(),chipBondary.x());
        std::uniform_real_distribution<double> m_distribution_y(chipOrigin.y(),chipBondary.y());
        for (int i = 0; i < number_of_clusters; ++i) {
            initial_centers.push_back(ophidian::geometry::Point(m_distribution_x(m_generator), m_distribution_y(m_generator)));
        }


        //DOD Sequential
        ophidian::KmeansDataOrientedDesign kmeansDOD (initial_centers);
        kmeansDOD.cluster_registers(flip_flop_positions, 10);

        std::vector<ophidian::geometry::Point> kmeansDODSequential_centers;
        kmeansDODSequential_centers.reserve(kmeansDOD.clusterCenters_.size());
        for(auto center_point : kmeansDOD.clusterCenters_)
            kmeansDODSequential_centers.push_back(center_point.first);

        //DOD Parallel
        ophidian::KmeansDataOrientedDesign kmeansDOD_parallel (initial_centers);
        kmeansDOD_parallel.cluster_registers(flip_flop_positions, 10);


        std::vector<ophidian::geometry::Point> kmeansDODParallel_centers;
        kmeansDODParallel_centers.reserve(kmeansDOD_parallel.clusterCenters_.size());
        for(auto center_point : kmeansDOD_parallel.clusterCenters_)
            kmeansDODParallel_centers.push_back(center_point.first);

        REQUIRE(kmeansDOD.clusters_.size() == kmeansDOD_parallel.clusters_.size());
        REQUIRE(std::is_permutation(kmeansDODSequential_centers.begin(), kmeansDODSequential_centers.end(), kmeansDODParallel_centers.begin(), point_comparison));
        REQUIRE(std::is_permutation(kmeansDOD.clusterElements_.begin(), kmeansDOD.clusterElements_.end(), kmeansDOD_parallel.clusterElements_.begin(), cluster_comparison));

        //DOD Sequential Rtree
        ophidian::KmeansDataOrientedDesign kmeansDOD_sequential_rtree (initial_centers);
        kmeansDOD_sequential_rtree.cluster_registers(flip_flop_positions, 10);

        std::vector<ophidian::geometry::Point> kmeansDODSequential_rtree_centers;
        kmeansDODSequential_rtree_centers.reserve(kmeansDOD_sequential_rtree.clusterCenters_.size());
        for(auto center_point : kmeansDOD_sequential_rtree.clusterCenters_)
            kmeansDODSequential_rtree_centers.push_back(center_point.first);

        REQUIRE(kmeansDOD.clusters_.size() == kmeansDOD_sequential_rtree.clusters_.size());
        REQUIRE(std::is_permutation(kmeansDODSequential_rtree_centers.begin(), kmeansDODSequential_rtree_centers.end(), kmeansDODSequential_centers.begin(), point_comparison));
        REQUIRE(std::is_permutation(kmeansDOD.clusterElements_.begin(), kmeansDOD.clusterElements_.end(), kmeansDOD_sequential_rtree.clusterElements_.begin(), cluster_comparison));

        //DOD Parallel Rtree
        ophidian::KmeansDataOrientedDesign kmeansDOD_parallel_rtree (initial_centers);
        kmeansDOD_parallel_rtree.cluster_registers(flip_flop_positions, 10);

        std::vector<ophidian::geometry::Point> kmeansDODParallel_rtree_centers;
        kmeansDODParallel_rtree_centers.reserve(kmeansDOD_parallel_rtree.clusterCenters_.size());
        for(auto center_point : kmeansDOD_parallel_rtree.clusterCenters_)
            kmeansDODParallel_rtree_centers.push_back(center_point.first);

        REQUIRE(kmeansDOD.clusters_.size() == kmeansDOD_parallel_rtree.clusters_.size());
        REQUIRE(std::is_permutation(kmeansDODParallel_rtree_centers.begin(), kmeansDODParallel_rtree_centers.end(), kmeansDODSequential_rtree_centers.begin(), point_comparison));
        REQUIRE(std::is_permutation(kmeansDOD.clusterElements_.begin(), kmeansDOD.clusterElements_.end(), kmeansDOD_parallel_rtree.clusterElements_.begin(), cluster_comparison));

        //OOD Sequential
        std::vector<ophidian::FlipFlop> flip_flops;
        flip_flops.reserve(flip_flop_positions.size());
        for(auto p : flip_flop_positions){
            flip_flops.push_back(ophidian::FlipFlop(p));
        }

        ophidian::KmeansObjectOrientedDesign kmeansOOD (initial_centers);
        kmeansOOD.cluster_registers(flip_flops, 10);

        std::vector<ophidian::geometry::Point> cluster_centers_OOD;
        cluster_centers_OOD.reserve(kmeansOOD.clusters_.size());

        std::vector<std::vector<ophidian::geometry::Point>> cluster_elements_OOD;
        cluster_elements_OOD.reserve(kmeansOOD.clusters_.size());

        for(ophidian::ClusterOOD cluster : kmeansOOD.clusters_){
            cluster_centers_OOD.push_back(cluster.center());

            std::vector<ophidian::geometry::Point> elements_positions;
            elements_positions.reserve(cluster.elements().size());
            for(ophidian::FlipFlop ff : cluster.elements()){
                elements_positions.push_back(ff.position());
            }
            cluster_elements_OOD.push_back(elements_positions);
        }
        REQUIRE(cluster_centers_OOD.size() == kmeansDOD.clusters_.size());


        std::vector<ophidian::geometry::Point> clusters_centers;
        clusters_centers.reserve(kmeansDOD.clusterCenters_.size());
        for(auto center_point : kmeansDOD.clusterCenters_)
            clusters_centers.push_back(center_point.first);

        REQUIRE(std::is_permutation(clusters_centers.begin(), clusters_centers.end(), cluster_centers_OOD.begin(), point_comparison));
        REQUIRE(std::is_permutation(kmeansDOD.clusterElements_.begin(), kmeansDOD.clusterElements_.end(), cluster_elements_OOD.begin(), cluster_comparison));

        //OOD Parallel
        ophidian::KmeansObjectOrientedDesign kmeansOOD_parallel (initial_centers);
        kmeansOOD_parallel.cluster_registers_paralel(flip_flops, 10);
        cluster_centers_OOD.clear();
        cluster_centers_OOD.reserve(kmeansOOD_parallel.clusters_.size());

        cluster_elements_OOD.clear();
        cluster_elements_OOD.reserve(kmeansOOD_parallel.clusters_.size());

        for(ophidian::ClusterOOD cluster : kmeansOOD_parallel.clusters_){
            cluster_centers_OOD.push_back(cluster.center());

            std::vector<ophidian::geometry::Point> elements_positions;
            elements_positions.reserve(cluster.elements().size());
            for(ophidian::FlipFlop ff : cluster.elements()){
                elements_positions.push_back(ff.position());
            }
            cluster_elements_OOD.push_back(elements_positions);
        }
        REQUIRE(cluster_centers_OOD.size() == kmeansDOD.clusters_.size());
        REQUIRE(std::is_permutation(kmeansDODSequential_centers.begin(), kmeansDODSequential_centers.end(), cluster_centers_OOD.begin(), point_comparison));
        REQUIRE(std::is_permutation(kmeansDOD.clusterElements_.begin(), kmeansDOD.clusterElements_.end(), cluster_elements_OOD.begin(), cluster_comparison));

        //OOD Sequential Rtree
        ophidian::KmeansObjectOrientedDesign kmeansOOD_sequential_rtree (initial_centers);
        kmeansOOD_sequential_rtree.cluster_registers_with_rtree(flip_flops, 10);
        cluster_centers_OOD.clear();
        cluster_centers_OOD.reserve(kmeansOOD_sequential_rtree.clusters_.size());

        cluster_elements_OOD.clear();
        cluster_elements_OOD.reserve(kmeansOOD_sequential_rtree.clusters_.size());

        for(ophidian::ClusterOOD cluster : kmeansOOD_sequential_rtree.clusters_){
            cluster_centers_OOD.push_back(cluster.center());

            std::vector<ophidian::geometry::Point> elements_positions;
            elements_positions.reserve(cluster.elements().size());
            for(ophidian::FlipFlop ff : cluster.elements()){
                elements_positions.push_back(ff.position());
            }
            cluster_elements_OOD.push_back(elements_positions);
        }
        REQUIRE(cluster_centers_OOD.size() == kmeansDOD.clusters_.size());
        REQUIRE(std::is_permutation(kmeansDODSequential_centers.begin(), kmeansDODSequential_centers.end(), cluster_centers_OOD.begin(), point_comparison));
        REQUIRE(std::is_permutation(kmeansDOD.clusterElements_.begin(), kmeansDOD.clusterElements_.end(), cluster_elements_OOD.begin(), cluster_comparison));

        //OOD Parallel Rtree
        ophidian::KmeansObjectOrientedDesign kmeansOOD_parallel_rtree (initial_centers);
        kmeansOOD_parallel_rtree.cluster_registers_with_rtree_paralel(flip_flops, 10);
        cluster_centers_OOD.clear();
        cluster_centers_OOD.reserve(kmeansOOD_parallel_rtree.clusters_.size());

        cluster_elements_OOD.clear();
        cluster_elements_OOD.reserve(kmeansOOD_parallel_rtree.clusters_.size());

        for(ophidian::ClusterOOD cluster : kmeansOOD_parallel_rtree.clusters_){
            cluster_centers_OOD.push_back(cluster.center());

            std::vector<ophidian::geometry::Point> elements_positions;
            elements_positions.reserve(cluster.elements().size());
            for(ophidian::FlipFlop ff : cluster.elements()){
                elements_positions.push_back(ff.position());
            }
            cluster_elements_OOD.push_back(elements_positions);
        }
        REQUIRE(cluster_centers_OOD.size() == kmeansDOD.clusters_.size());
        REQUIRE(std::is_permutation(kmeansDODSequential_centers.begin(), kmeansDODSequential_centers.end(), cluster_centers_OOD.begin(), point_comparison));
        REQUIRE(std::is_permutation(kmeansDOD.clusterElements_.begin(), kmeansDOD.clusterElements_.end(), cluster_elements_OOD.begin(), cluster_comparison));
    }
}

