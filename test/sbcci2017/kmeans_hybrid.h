#ifndef KMEANSHYBRID_H
#define KMEANSHYBRID_H



#include <omp.h>

#include "ophidian/entity_system/EntitySystem.h"
#include "ophidian/entity_system/Property.h"
#include "ophidian/geometry/Models.h"

namespace ophidian {



class ClusterHybrid
{
private:
    geometry::Point center_;
    std::vector<geometry::Point> elements_;
public:
    ClusterHybrid(const geometry::Point &center);

    geometry::Point & center();
    void setCenter(const geometry::Point &center);
    std::vector<geometry::Point> elements() const;
    void setElements(const std::vector<geometry::Point> &elements);


    void insertElement(const geometry::Point &element);
    void clear();
    int size();
};

class KmeansHybrid
{

private:
    std::default_random_engine m_generator;
    std::uniform_real_distribution<double> m_distribution_x;
    std::uniform_real_distribution<double> m_distribution_y;

    using rtree_node = std::pair<geometry::Point, ClusterHybrid*>;
    using rtree = boost::geometry::index::rtree<rtree_node, boost::geometry::index::rstar<16>>;

public:
    KmeansHybrid(geometry::Point chipOrigin, geometry::Point chipBondary, unsigned k = 50);
    KmeansHybrid(const std::vector<geometry::Point> &centers);

    std::vector<ClusterHybrid> clusters_;

    void cluster_registers(const std::vector<geometry::Point> & flip_flops, unsigned iterations = 10);
    void cluster_registers_parallel(const std::vector<geometry::Point> & flip_flops, unsigned iterations = 10);
    void cluster_registers_with_rtree(const std::vector<geometry::Point> & flip_flops, unsigned iterations = 10);
    void cluster_registers_with_rtree_parallel(const std::vector<geometry::Point> & flip_flops, unsigned iterations = 10);
};

}

#endif // KMEANSHYBRID_H
