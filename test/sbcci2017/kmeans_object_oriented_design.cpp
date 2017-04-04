#include "kmeans_object_oriented_design.h"
#include <chrono>

namespace ophidian {


//-------------------------  Class ClusterElement -------------------------
ClusterOOD *FlipFlop::clusterBest() const
{
    return clusterBest_;
}

void FlipFlop::setClusterBest(ClusterOOD *clusterBest)
{
    clusterBest_ = clusterBest;
}

geometry::Point FlipFlop::position() const
{
    return position_;
}

void FlipFlop::setPosition(const geometry::Point &position)
{
    position_ = position;
}




//-------------------------  Class Cluster -------------------------
ophidian::ClusterOOD::ClusterOOD(const ophidian::geometry::Point &center) : center_(center)
{

}

std::vector<FlipFlop> ClusterOOD::elements() const
{
    return elements_;
}

void ClusterOOD::elements(const std::vector<FlipFlop> &clusterElements)
{
    elements_ = clusterElements;
}


geometry::Point ClusterOOD::center() const
{
    return center_;
}

void ClusterOOD::center(const geometry::Point &clusterCenter)
{
    center_ = clusterCenter;
}

void ClusterOOD::insertElement(const FlipFlop &element)
{
    elements_.push_back(element);
}

void ClusterOOD::clear()
{
    elements().clear();
}

int ClusterOOD::size()
{
    return elements_.size();
}


//-------------------------  Class KmeansObjectOrientedDesign -------------------------
KmeansObjectOrientedDesign::KmeansObjectOrientedDesign(geometry::Point chipOrigin, geometry::Point chipBondary, unsigned k) :
    m_distribution_x(chipOrigin.x(),chipBondary.x()), m_distribution_y(chipOrigin.y(),chipBondary.y())
{
    clusters_.reserve(k);
    for (int i = 0; i < k; ++i) {
        clusters_.push_back(ClusterOOD(geometry::Point(m_distribution_x(m_generator), m_distribution_y(m_generator))));
    }
}

KmeansObjectOrientedDesign::KmeansObjectOrientedDesign(const std::vector<geometry::Point> &centers)
{
    clusters_.reserve(centers.size());
    for(auto p : centers){
        clusters_.push_back(ClusterOOD(p));
    }
}

std::vector<ClusterOOD> KmeansObjectOrientedDesign::clusters() const
{
    return clusters_;
}

void KmeansObjectOrientedDesign::setClusters(const std::vector<ClusterOOD> &clusters)
{
    clusters_ = clusters;
}

void KmeansObjectOrientedDesign::cluster_registers(std::vector<FlipFlop> &flip_flops, unsigned iterations)
{
//    std::chrono::high_resolution_clock::time_point time_start, time_end;
//    std::chrono::high_resolution_clock::duration d1, d2;
    for (int i = 0; i < iterations; ++i) {

        for(auto & cluster : clusters()){
            cluster.clear();
        }



//        time_start = std::chrono::high_resolution_clock::now();
        for (auto & flip_flop : flip_flops) {
            ClusterOOD * cluster_best;
            double cost_best = std::numeric_limits<double>::max();
            for (auto & cluster : clusters_) {
                geometry::Point center = cluster.center();

                double distanceX = (flip_flop.position().x() - center.x()) * (flip_flop.position().x() - center.x());
                double distanceY = (flip_flop.position().y() - center.y()) * (flip_flop.position().y() - center.y());
                double cost = std::sqrt(distanceX + distanceY);

                if(cost < cost_best){
                    cost_best = cost;
                    cluster_best = &cluster;
                }
            }
           cluster_best->insertElement(flip_flop);
        }
//        time_end = std::chrono::high_resolution_clock::now();
//        d1 += time_end - time_start;

//        time_start = std::chrono::high_resolution_clock::now();
        for (ClusterOOD & cluster : clusters_) {
            double x_c = 0, y_c = 0;
            for(auto p : cluster.elements()){
                x_c += p.position().x();
                y_c += p.position().y();
            }
            x_c = x_c / (double)cluster.elements().size();
            y_c = y_c / (double)cluster.elements().size();
            cluster.center(geometry::Point(x_c, y_c));
        }
//        time_end = std::chrono::high_resolution_clock::now();
//        d2 += time_end - time_start;
    }
//    std::cout<<"for 1: "<<std::chrono::duration_cast<std::chrono::milliseconds>(d1).count()<<" ms."<<std::endl;
//    std::cout<<"for 2: "<<std::chrono::duration_cast<std::chrono::milliseconds>(d2).count()<<" ms."<<std::endl;
}

void KmeansObjectOrientedDesign::cluster_registers_with_rtree( std::vector<FlipFlop> &flip_flops, unsigned iterations)
{

}

void KmeansObjectOrientedDesign::cluster_registers_paralel(std::vector<FlipFlop> &flip_flops, unsigned iterations)
{
    for (int i = 0; i < iterations; ++i) {

#pragma omp parallel for
        for(auto cluster = clusters_.begin(); cluster < clusters_.end(); ++cluster){
            cluster->clear();
        }

#pragma omp parallel for
        for (unsigned flip_flop_index = 0; flip_flop_index < flip_flops.size(); ++flip_flop_index) {
            FlipFlop &flip_flop = flip_flops.at(flip_flop_index);

            ClusterOOD * cluster_best;
            double cost_best = std::numeric_limits<double>::max();
            for (auto & cluster : clusters_) {
                geometry::Point center = cluster.center();

                double distanceX = (flip_flop.position().x() - center.x()) * (flip_flop.position().x() - center.x());
                double distanceY = (flip_flop.position().y() - center.y()) * (flip_flop.position().y() - center.y());
                double cost = std::sqrt(distanceX + distanceY);

                if(cost < cost_best){
                    cost_best = cost;
                    cluster_best = &cluster;
                }
            }
           flip_flop.setClusterBest(cluster_best);
        }

        for(unsigned flip_flop_index = 0; flip_flop_index < flip_flops.size(); ++flip_flop_index){
            auto &flip_flop = flip_flops.at(flip_flop_index);
            auto cluster = flip_flop.clusterBest();
            cluster->insertElement(flip_flop);
        }

#pragma omp parallel for
        for (auto cluster = clusters_.begin(); cluster < clusters_.end(); ++cluster) {
            double x_c = 0, y_c = 0;
            for(auto p : cluster->elements()){
                x_c += p.position().x();
                y_c += p.position().y();
            }
            x_c = x_c / (double)cluster->elements().size();
            y_c = y_c / (double)cluster->elements().size();
            cluster->center(geometry::Point(x_c, y_c));
        }
    }
}

void KmeansObjectOrientedDesign::cluster_registers_with_rtree_paralel( std::vector<FlipFlop> &flip_flops, unsigned iterations)
{

}


}
