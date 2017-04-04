#ifndef KMEANSDATAORIENTEDDESIGN2_H
#define KMEANSDATAORIENTEDDESIGN2_H

#include <omp.h>

#include "ophidian/entity_system/EntitySystem.h"
#include "ophidian/entity_system/Property.h"
#include "ophidian/geometry/Models.h"
#include "ophidian/entity_system/Aggregation.h"

namespace ophidian {

class FlipFlopDOD : public entity_system::EntityBase {
public:
    using entity_system::EntityBase::EntityBase;
};

class Cluster2 : public entity_system::EntityBase
{
public:
    using entity_system::EntityBase::EntityBase;
};

class KmeansDataOrientedDesign2
{

private:
    std::default_random_engine m_generator;
    std::uniform_real_distribution<double> m_distribution_x;
    std::uniform_real_distribution<double> m_distribution_y;

    using rtree_node = std::pair<geometry::Point, Cluster2>;
    using rtree = boost::geometry::index::rtree<rtree_node, boost::geometry::index::rstar<16>>;

public:
    KmeansDataOrientedDesign2(entity_system::EntitySystem<FlipFlopDOD> &flip_flops, geometry::Point chipOrigin, geometry::Point chipBondary, unsigned k = 50);
    KmeansDataOrientedDesign2(entity_system::EntitySystem<FlipFlopDOD> &flip_flops, const std::vector<geometry::Point> &centers);

    entity_system::EntitySystem<Cluster2> clusters_;
    entity_system::Property<Cluster2, geometry::Point> clusterCenters_;
    entity_system::Property<Cluster2, std::vector<geometry::Point>> clusterElements_;
    entity_system::Aggregation<Cluster2, FlipFlopDOD> cluster2FlipFlops_;

    void cluster_registers(const entity_system::EntitySystem<FlipFlopDOD> &flip_flops, entity_system::Property<FlipFlopDOD, geometry::Point> &flip_flop_positions, unsigned iterations = 10);
    void cluster_registers_parallel(const std::vector<geometry::Point> & flip_flops, unsigned iterations = 10);
    void cluster_registers_with_rtree(const entity_system::EntitySystem<FlipFlopDOD> &flip_flops, entity_system::Property<FlipFlopDOD, geometry::Point> &flip_flop_positions, unsigned iterations = 10);
};

}

#endif // KMEANSDATAORIENTEDDESIGN2_H
