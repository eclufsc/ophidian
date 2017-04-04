#include "kmeansdataorienteddesign2.h"
#include <boost/geometry/index/rtree.hpp>
#include <chrono>

namespace ophidian {

KmeansDataOrientedDesign2::KmeansDataOrientedDesign2(entity_system::EntitySystem<FlipFlopDOD> &flip_flops, geometry::Point chipOrigin, geometry::Point chipBondary, unsigned k) :
    clusterCenters_(clusters_), clusterElements_(clusters_), m_distribution_x(chipOrigin.x(),chipBondary.x()), m_distribution_y(chipOrigin.y(),chipBondary.y()), cluster2FlipFlops_(clusters_, flip_flops)
{
    clusters_.reserve(k);
    for (int i = 0; i < k; ++i) {
        auto cluster = clusters_.add();
        clusterCenters_[cluster] = geometry::Point(m_distribution_x(m_generator), m_distribution_y(m_generator));
    }
}

KmeansDataOrientedDesign2::KmeansDataOrientedDesign2(entity_system::EntitySystem<FlipFlopDOD> &flip_flops, const std::vector<geometry::Point> &centers) :
    clusterCenters_(clusters_), clusterElements_(clusters_), cluster2FlipFlops_(clusters_, flip_flops)
{
    for(auto p : centers){
        auto cluster = clusters_.add();
        clusterCenters_[cluster] = p;
    }

}


void KmeansDataOrientedDesign2::cluster_registers(const entity_system::EntitySystem<FlipFlopDOD> &flip_flops, entity_system::Property<FlipFlopDOD, geometry::Point> & flip_flop_positions, unsigned iterations)
{
    for (int i = 0; i < iterations; ++i) {

        for (auto & flip_flop : flip_flops) {
            auto flip_flop_point = flip_flop_positions[flip_flop];
            Cluster2 cluster_best;
            double cost_best = std::numeric_limits<double>::max();
            for (auto & cluster : clusters_) {
                geometry::Point center = clusterCenters_[cluster];

                double distanceX = (flip_flop_point.x() - center.x()) * (flip_flop_point.x() - center.x());
                double distanceY = (flip_flop_point.y() - center.y()) * (flip_flop_point.y() - center.y());
                double cost = std::sqrt(distanceX + distanceY);

                if(cost < cost_best){
                    cost_best = cost;
                    cluster_best = cluster;
                }
            }
            cluster2FlipFlops_.addAssociation(cluster_best, flip_flop);
        }

        for (auto & cluster : clusters_) {
            double x_c = 0, y_c = 0;
            for (auto flip_flop : cluster2FlipFlops_.parts(cluster)) {
                auto p = flip_flop_positions[flip_flop];
                x_c += p.x();
                y_c += p.y();
            }
            x_c = x_c / (double)cluster2FlipFlops_.numParts(cluster);
            y_c = y_c / (double)cluster2FlipFlops_.numParts(cluster);
            clusterCenters_[cluster] = geometry::Point(x_c, y_c);
        }

        cluster2FlipFlops_.clear();
    }
}

void KmeansDataOrientedDesign2::cluster_registers_parallel(const std::vector<geometry::Point> &flip_flops, unsigned iterations)
{
    for (int i = 0; i < iterations; ++i) {

#pragma omp parallel for
        for (auto elements_to_cluster = clusterElements_.begin(); elements_to_cluster < clusterElements_.end(); ++elements_to_cluster) {
            elements_to_cluster->clear();
        }

        std::vector<Cluster2> flip_flop_to_cluster;
        flip_flop_to_cluster.resize(flip_flops.size());
#pragma omp parallel for
        for (unsigned flip_flop_index = 0; flip_flop_index < flip_flops.size(); ++flip_flop_index) {
            auto flip_flop = flip_flops.at(flip_flop_index);

            Cluster2 cluster_best;
            double cost_best = std::numeric_limits<double>::max();
            for (auto & cluster : clusters_) {
                geometry::Point center = clusterCenters_[cluster];

                double distanceX = (flip_flop.x() - center.x()) * (flip_flop.x() - center.x());
                double distanceY = (flip_flop.y() - center.y()) * (flip_flop.y() - center.y());
                double cost = std::sqrt(distanceX + distanceY);

                if(cost < cost_best){
                    cost_best = cost;
                    cluster_best = cluster;
                }
            }
            flip_flop_to_cluster.at(flip_flop_index) = cluster_best;
        }

        for(unsigned flip_flop_index = 0; flip_flop_index < flip_flops.size(); ++flip_flop_index){
            auto flip_flop = flip_flops.at(flip_flop_index);
            auto cluster = flip_flop_to_cluster.at(flip_flop_index);
            clusterElements_[cluster].push_back(flip_flop);
        }

#pragma omp parallel for
        for (auto cluster = clusters_.begin(); cluster < clusters_.end(); ++cluster) {
            double x_c = 0, y_c = 0;
            for(auto p : clusterElements_[*cluster]){
                x_c += p.x();
                y_c += p.y();
            }
            x_c = x_c / (double)clusterElements_[*cluster].size();
            y_c = y_c / (double)clusterElements_[*cluster].size();
            clusterCenters_[*cluster] = geometry::Point(x_c, y_c);
        }
    }
}

void KmeansDataOrientedDesign2::cluster_registers_with_rtree(const entity_system::EntitySystem<FlipFlopDOD> &flip_flops, entity_system::Property<FlipFlopDOD, geometry::Point> & flip_flop_positions, unsigned iterations)
{
    for (int i = 0; i < iterations; ++i) {
        std::cout << "iteration " << i << std::endl;
        std::cout << "number of clusters " << clusters_.size() << std::endl;

        rtree clusters_rtree;
        for (auto & cluster : clusters_) {
            clusters_rtree.insert(rtree_node(clusterCenters_[cluster], cluster));
        }

        for (auto & flip_flop : flip_flops) {
            std::vector<rtree_node> closest_nodes;
            clusters_rtree.query(boost::geometry::index::nearest(flip_flop_positions[flip_flop], 1), std::back_inserter(closest_nodes));
            auto closest_cluster = closest_nodes.front().second;
            cluster2FlipFlops_.addAssociation(closest_cluster, flip_flop);
//            clusterElements_[closest_cluster].push_back(flip_flop);
        }
        for (auto & cluster : clusters_) {
            double x_c = 0, y_c = 0;
//            for(auto p : clusterElements_[cluster]){
            for (auto flip_flop : cluster2FlipFlops_.parts(cluster)) {
                auto p = flip_flop_positions[flip_flop];
                x_c += p.x();
                y_c += p.y();
            }
//            x_c = x_c / (double)clusterElements_[cluster].size();
//            y_c = y_c / (double)clusterElements_[cluster].size();
            x_c = x_c / (double)cluster2FlipFlops_.numParts(cluster);
            y_c = y_c / (double)cluster2FlipFlops_.numParts(cluster);
            clusterCenters_[cluster] = geometry::Point(x_c, y_c);
        }
//        time_end = std::chrono::high_resolution_clock::now();
//        d2 += time_end - time_start;

        cluster2FlipFlops_.clear();
    }
}

}
