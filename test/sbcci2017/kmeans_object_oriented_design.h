#ifndef KMEANSOBJECTORIENTEDDESIGN_H
#define KMEANSOBJECTORIENTEDDESIGN_H

#include "ophidian/geometry/Models.h"

namespace ophidian {


class FlipFlop
{
private:
    geometry::Point position_;
public:
    FlipFlop(geometry::Point & p): position_(p) {}

    geometry::Point position() const;
    void setPosition(const geometry::Point &position);
};

class ClusterOOD
{
private:
    geometry::Point clusterCenter_;
    std::vector<FlipFlop> clusterElements_;
public:
    ClusterOOD(geometry::Point & center);

    std::vector<FlipFlop> clusterElements() const;
    void setClusterElements(const std::vector<FlipFlop> &clusterElements);
    geometry::Point clusterCenter() const;
    void setClusterCenter(const geometry::Point &clusterCenter);

    void insertElement(const FlipFlop &element);
    void clear();
    int size();
};


class KmeansObjectOrientedDesign
{
public:
    KmeansObjectOrientedDesign();

    void cluster_registers(const std::vector<geometry::Point> & flip_flops, unsigned iterations = 10);
    void cluster_registers_with_rtree(const std::vector<geometry::Point> & flip_flops, unsigned iterations = 10);
};


}
#endif // KMEANSOBJECTORIENTEDDESIGN_H
