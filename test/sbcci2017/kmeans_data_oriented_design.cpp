#include "kmeans_data_oriented_design.h"
#include <boost/geometry/index/rtree.hpp>

namespace ophidian {

KmeansDataOrientedDesign::KmeansDataOrientedDesign(geometry::Point chipOrigin, geometry::Point chipBondary, unsigned k) :
    clusterCenters_(clusters_), clusterElements_(clusters_), m_distribution_x(chipOrigin.x(),chipBondary.x()), m_distribution_y(chipOrigin.y(),chipBondary.y())
{
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
    for (int i = 0; i < iterations; ++i) {
        std::cout << "iteration: " << i <<std::endl;

        //paralel
        std::vector<Cluster> flip_flop_to_cluster;
        flip_flop_to_cluster.reserve(flip_flops.size());
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
            flip_flop_to_cluster.at(flip_flop_index) = best_cluster;
        }

        for(unsigned flip_flop_index = 0; flip_flop_index < flip_flops.size(); ++flip_flop_index){
            auto cluster = flip_flop_to_cluster.at(flip_flop_index);
            auto flip_flop = flip_flops.at(flip_flop_index);
            clusterElements_[cluster].push_back(flip_flop);
        }

        //paralel
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
