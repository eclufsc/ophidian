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
ophidian::ClusterOOD::ClusterOOD(ophidian::geometry::Point &center) : clusterCenter_(center)
{

}

std::vector<FlipFlop> ClusterOOD::clusterElements() const
{
    return clusterElements_;
}

void ClusterOOD::setClusterElements(const std::vector<FlipFlop> &clusterElements)
{
    clusterElements_ = clusterElements;
}


geometry::Point ClusterOOD::clusterCenter() const
{
    return clusterCenter_;
}

void ClusterOOD::setClusterCenter(const geometry::Point &clusterCenter)
{
    clusterCenter_ = clusterCenter;
}

void ClusterOOD::insertElement(const FlipFlop &element)
{
    clusterElements_.push_back(element);
}

void ClusterOOD::clear()
{
    clusterElements().clear();
}

int ClusterOOD::size()
{
    return clusterElements_.size();
}




//-------------------------  Class KmeansObjectOrientedDesign -------------------------
KmeansObjectOrientedDesign::KmeansObjectOrientedDesign()
{

}

}
