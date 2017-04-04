#include "kmeans_hybrid.h"
namespace ophidian {

//_--------------------class ClusterHybrid --------------------------
ClusterHybrid::ClusterHybrid(const geometry::Point &center) : center_(center)
{

}

geometry::Point & ClusterHybrid::center()
{
    return center_;
}

void ClusterHybrid::setCenter(const geometry::Point &center)
{
    center_ = center;
}

std::vector<geometry::Point> ClusterHybrid::elements() const
{
    return elements_;
}

void ClusterHybrid::setElements(const std::vector<geometry::Point> &elements)
{
    elements_ = elements;
}

void ClusterHybrid::insertElement(const geometry::Point &element)
{
    elements_.push_back(element);
}

void ClusterHybrid::clear()
{
    elements_.clear();
}

int ClusterHybrid::size()
{
    elements_.size();
}

//-------------------------class KmeansHybrid -------------------
KmeansHybrid::KmeansHybrid(geometry::Point chipOrigin, geometry::Point chipBondary, unsigned k) :
    m_distribution_x(chipOrigin.x(),chipBondary.x()), m_distribution_y(chipOrigin.y(),chipBondary.y())
{
    clusters_.reserve(k);
    for (int i = 0; i < k; ++i) {
        clusters_.push_back(ClusterHybrid(geometry::Point(m_distribution_x(m_generator), m_distribution_y(m_generator))));
    }
}

KmeansHybrid::KmeansHybrid(const std::vector<geometry::Point> &centers)
{
    clusters_.reserve(centers.size());
    for(auto p : centers){
        clusters_.push_back(ClusterHybrid(p));
    }
}

void KmeansHybrid::cluster_registers(const std::vector<geometry::Point> &flip_flops, unsigned iterations)
{
    for (int i = 0; i < iterations; ++i) {

        for (auto & cluster : clusters_) {
            cluster.clear();
        }

        for (auto & flip_flop : flip_flops) {
            ClusterHybrid * cluster_best;
            double cost_best = std::numeric_limits<double>::max();
            for (auto & cluster : clusters_) {
                geometry::Point center = cluster.center();

                double distanceX = (flip_flop.x() - center.x()) * (flip_flop.x() - center.x());
                double distanceY = (flip_flop.y() - center.y()) * (flip_flop.y() - center.y());
                double cost = std::sqrt(distanceX + distanceY);

                if(cost < cost_best){
                    cost_best = cost;
                    cluster_best = &cluster;
                }
            }
            cluster_best->insertElement(flip_flop);
        }

        for (ClusterHybrid & cluster : clusters_) {
            if(cluster.elements().size() != 0){
                double x_c = 0, y_c = 0;
                for(auto p : cluster.elements()){
                    x_c += p.x();
                    y_c += p.y();
                }
                x_c = x_c / (double)cluster.elements().size();
                y_c = y_c / (double)cluster.elements().size();
                cluster.setCenter(geometry::Point(x_c, y_c));
            }
        }
    }
}

void KmeansHybrid::cluster_registers_parallel(const std::vector<geometry::Point> &flip_flops, unsigned iterations)
{
    for (int i = 0; i < iterations; ++i) {
#pragma omp parallel for
        for (auto cluster = clusters_.begin(); cluster < clusters_.end(); ++cluster) {
            cluster->clear();
        }
        std::vector<ClusterHybrid*> flip_flop_to_cluster;
        flip_flop_to_cluster.resize(flip_flops.size());
#pragma omp parallel for
        for (unsigned flip_flop_index = 0; flip_flop_index < flip_flops.size(); ++flip_flop_index) {
            auto flip_flop = flip_flops.at(flip_flop_index);
            ClusterHybrid * cluster_best;
            double cost_best = std::numeric_limits<double>::max();
            for (auto & cluster : clusters_) {
                geometry::Point center = cluster.center();

                double distanceX = (flip_flop.x() - center.x()) * (flip_flop.x() - center.x());
                double distanceY = (flip_flop.y() - center.y()) * (flip_flop.y() - center.y());
                double cost = std::sqrt(distanceX + distanceY);

                if(cost < cost_best){
                    cost_best = cost;
                    cluster_best = &cluster;
                }
            }
            flip_flop_to_cluster.at(flip_flop_index) = cluster_best;
        }

        for(unsigned flip_flop_index = 0; flip_flop_index < flip_flops.size(); ++flip_flop_index){
            geometry::Point flip_flop = flip_flops.at(flip_flop_index);
            ClusterHybrid * cluster = flip_flop_to_cluster.at(flip_flop_index);
            cluster->insertElement(flip_flop);
        }

#pragma omp parallel for
        for (auto cluster = clusters_.begin(); cluster < clusters_.end(); ++cluster) {
            if(cluster->elements().size() != 0){
                double x_c = 0, y_c = 0;
                for(auto p : cluster->elements()){
                    x_c += p.x();
                    y_c += p.y();
                }
                x_c = x_c / (double)cluster->elements().size();
                y_c = y_c / (double)cluster->elements().size();
                cluster->setCenter(geometry::Point(x_c, y_c));
            }
        }
    }
}

void KmeansHybrid::cluster_registers_with_rtree(const std::vector<geometry::Point> &flip_flops, unsigned iterations)
{
    for (int i = 0; i < iterations; ++i) {
        rtree clusters_rtree;
        for (auto & cluster : clusters_) {
            clusters_rtree.insert(rtree_node(cluster.center(), &cluster));
            cluster.clear();
        }

        for (auto & flip_flop : flip_flops) {
            std::vector<rtree_node> closest_nodes;
            clusters_rtree.query(boost::geometry::index::nearest(flip_flop, 1), std::back_inserter(closest_nodes));
            ClusterHybrid * closest_cluster = closest_nodes.front().second;
            closest_cluster->insertElement(flip_flop);
        }

        for (ClusterHybrid & cluster : clusters_) {
            if(cluster.elements().size() != 0){
                double x_c = 0, y_c = 0;
                for(auto p : cluster.elements()){
                    x_c += p.x();
                    y_c += p.y();
                }
                x_c = x_c / (double)cluster.elements().size();
                y_c = y_c / (double)cluster.elements().size();
                cluster.setCenter(geometry::Point(x_c, y_c));
            }
        }
    }
}

void KmeansHybrid::cluster_registers_with_rtree_parallel(const std::vector<geometry::Point> &flip_flops, unsigned iterations)
{
    for (int i = 0; i < iterations; ++i) {
        rtree clusters_rtree;
        for (auto & cluster : clusters_) {
            clusters_rtree.insert(rtree_node(cluster.center(), &cluster));
            cluster.clear();
        }
        std::vector<ClusterHybrid*> flip_flop_to_cluster;
        flip_flop_to_cluster.resize(flip_flops.size());
#pragma omp parallel for
        for (unsigned flip_flop_index = 0; flip_flop_index < flip_flops.size(); ++flip_flop_index) {
            auto flip_flop_position = flip_flops.at(flip_flop_index);
            std::vector<rtree_node> closest_nodes;
            clusters_rtree.query(boost::geometry::index::nearest(flip_flop_position, 1), std::back_inserter(closest_nodes));
            ClusterHybrid * closest_cluster = closest_nodes.front().second;
            flip_flop_to_cluster.at(flip_flop_index) = closest_cluster;
        }

        for(unsigned flip_flop_index = 0; flip_flop_index < flip_flops.size(); ++flip_flop_index){
            auto flip_flop_position = flip_flops.at(flip_flop_index);
            ClusterHybrid * cluster = flip_flop_to_cluster.at(flip_flop_index);
            cluster->insertElement(flip_flop_position);
        }

#pragma omp parallel for
        for (auto cluster = clusters_.begin(); cluster < clusters_.end(); ++cluster) {
            if(cluster->elements().size() != 0){
                double x_c = 0, y_c = 0;
                for(auto p : cluster->elements()){
                    x_c += p.x();
                    y_c += p.y();
                }
                x_c = x_c / (double)cluster->elements().size();
                y_c = y_c / (double)cluster->elements().size();
                cluster->setCenter(geometry::Point(x_c, y_c));
            }
        }
    }
}

}
