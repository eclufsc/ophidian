#ifndef KMEANSDATAORIENTEDDESIGN_H
#define KMEANSDATAORIENTEDDESIGN_H

#include <omp.h>

#include "ophidian/entity_system/EntitySystem.h"
#include "ophidian/entity_system/Property.h"
#include "ophidian/geometry/Models.h"

namespace ophidian {



class Cluster : public entity_system::EntityBase
{
public:
    using entity_system::EntityBase::EntityBase;
};

class KmeansDataOrientedDesign
{

private:
    std::default_random_engine m_generator;
    std::uniform_real_distribution<double> m_distribution_x;
    std::uniform_real_distribution<double> m_distribution_y;

    using rtree_node = std::pair<geometry::Point, Cluster>;
    using rtree = boost::geometry::index::rtree<rtree_node, boost::geometry::index::rstar<16>>;

public:
    KmeansDataOrientedDesign(geometry::Point chipOrigin, geometry::Point chipBondary, unsigned k = 50);
    KmeansDataOrientedDesign(const std::vector<geometry::Point> &centers);

    entity_system::EntitySystem<Cluster> clusters_;
    entity_system::Property<Cluster, geometry::Point> clusterCenters_;
    entity_system::Property<Cluster, std::vector<geometry::Point>> clusterElements_;

    void cluster_registers(const std::vector<geometry::Point> & flip_flops, unsigned iterations = 10);
    void cluster_registers_parallel(const std::vector<geometry::Point> & flip_flops, unsigned iterations = 10);
    void cluster_registers_with_rtree(const std::vector<geometry::Point> & flip_flops, unsigned iterations = 10);
};

}

#endif // KMEANSDATAORIENTEDDESIGN_H
