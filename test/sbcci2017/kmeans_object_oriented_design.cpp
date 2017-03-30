#include "kmeans_object_oriented_design.h"

namespace ophidian {


//-------------------------  Class ClusterElement -------------------------
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

void KmeansObjectOrientedDesign::cluster_registers(const std::vector<geometry::Point> &flip_flops, unsigned iterations)
{
    for (int i = 0; i < iterations; ++i) {
        std::vector<ClusterOOD*> flip_flop_to_cluster;
        flip_flop_to_cluster.resize(flip_flops.size());

        for (unsigned flip_flop_index = 0; flip_flop_index < flip_flops.size(); ++flip_flop_index) {
            auto flip_flop = flip_flops.at(flip_flop_index);

            ClusterOOD * cluster_best;
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
            auto cluster = flip_flop_to_cluster.at(flip_flop_index);
            auto flip_flop = flip_flops.at(flip_flop_index);
            cluster->insertElement(flip_flop);
        }

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
    }
}

void KmeansObjectOrientedDesign::cluster_registers_with_rtree(const std::vector<geometry::Point> &flip_flops, unsigned iterations)
{

}

void KmeansObjectOrientedDesign::cluster_registers_paralel(const std::vector<geometry::Point> &flip_flops, unsigned iterations)
{

}

void KmeansObjectOrientedDesign::cluster_registers_with_rtree_paralel(const std::vector<geometry::Point> &flip_flops, unsigned iterations)
{

}


}
