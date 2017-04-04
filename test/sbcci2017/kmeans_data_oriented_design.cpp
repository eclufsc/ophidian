#include "kmeans_data_oriented_design.h"
#include <boost/geometry/index/rtree.hpp>
#include <chrono>

namespace ophidian {

KmeansDataOrientedDesign::KmeansDataOrientedDesign(geometry::Point chipOrigin, geometry::Point chipBondary, unsigned k) :
    clusterCenters_(clusters_), clusterElements_(clusters_), m_distribution_x(chipOrigin.x(),chipBondary.x()), m_distribution_y(chipOrigin.y(),chipBondary.y())
{
    clusters_.reserve(k);
    for (int i = 0; i < k; ++i) {
        auto cluster = clusters_.add();
        clusterCenters_[cluster] = geometry::Point(m_distribution_x(m_generator), m_distribution_y(m_generator));
    }
}

KmeansDataOrientedDesign::KmeansDataOrientedDesign(const std::vector<geometry::Point> &centers) :
    clusterCenters_(clusters_), clusterElements_(clusters_)
{
    for(auto p : centers){
        auto cluster = clusters_.add();
        clusterCenters_[cluster] = p;
    }

}


void KmeansDataOrientedDesign::cluster_registers(const std::vector<geometry::Point> &flip_flops, unsigned iterations)
{
    std::chrono::high_resolution_clock::time_point time_start, time_end;
    std::chrono::high_resolution_clock::duration d1, d2;
    for (int i = 0; i < iterations; ++i) {

        for (auto & elements_to_cluster : clusterElements_) {
            elements_to_cluster.clear();
        }

        time_start = std::chrono::high_resolution_clock::now();
        for (auto & flip_flop : flip_flops) {
            Cluster cluster_best;
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
            clusterElements_[cluster_best].push_back(flip_flop);
        }
        time_end = std::chrono::high_resolution_clock::now();
        d1 += time_end - time_start;

        time_start = std::chrono::high_resolution_clock::now();
        for (auto & cluster : clusters_) {
            double x_c = 0, y_c = 0;
            for(auto p : clusterElements_[cluster]){
                x_c += p.x();
                y_c += p.y();
            }
            x_c = x_c / (double)clusterElements_[cluster].size();
            y_c = y_c / (double)clusterElements_[cluster].size();
            clusterCenters_[cluster] = geometry::Point(x_c, y_c);
        }
        time_end = std::chrono::high_resolution_clock::now();
        d2 += time_end - time_start;
    }
    std::cout<<"for 1: "<<std::chrono::duration_cast<std::chrono::milliseconds>(d1).count()<<" ms."<<std::endl;
    std::cout<<"for 2: "<<std::chrono::duration_cast<std::chrono::milliseconds>(d2).count()<<" ms."<<std::endl;
}

void KmeansDataOrientedDesign::cluster_registers_parallel(const std::vector<geometry::Point> &flip_flops, unsigned iterations)
{
    for (int i = 0; i < iterations; ++i) {

#pragma omp parallel for
        for (auto elements_to_cluster = clusterElements_.begin(); elements_to_cluster < clusterElements_.end(); ++elements_to_cluster) {
            elements_to_cluster->clear();
        }

        std::vector<Cluster> flip_flop_to_cluster;
        flip_flop_to_cluster.resize(flip_flops.size());
#pragma omp parallel for
        for (unsigned flip_flop_index = 0; flip_flop_index < flip_flops.size(); ++flip_flop_index) {
            auto flip_flop = flip_flops.at(flip_flop_index);

            Cluster cluster_best;
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

void KmeansDataOrientedDesign::cluster_registers_with_rtree(const std::vector<geometry::Point> &flip_flops, unsigned iterations)
{
    for (int i = 0; i < iterations; ++i) {
        rtree clusters_rtree;
        for (auto & cluster : clusters_) {
            clusters_rtree.insert(rtree_node(clusterCenters_[cluster], cluster));
            clusterElements_[cluster].clear();
        }

        for (auto & flip_flop : flip_flops) {
            std::vector<rtree_node> closest_nodes;
            clusters_rtree.query(boost::geometry::index::nearest(flip_flop, 1), std::back_inserter(closest_nodes));
            auto closest_cluster = closest_nodes.front().second;
            clusterElements_[closest_cluster].push_back(flip_flop);
        }
        for (auto & cluster : clusters_) {
            double x_c = 0, y_c = 0;
            for(auto p : clusterElements_[cluster]){
                x_c += p.x();
                y_c += p.y();
            }
            x_c = x_c / (double)clusterElements_[cluster].size();
            y_c = y_c / (double)clusterElements_[cluster].size();
            clusterCenters_[cluster] = geometry::Point(x_c, y_c);
        }
    }
}

}
