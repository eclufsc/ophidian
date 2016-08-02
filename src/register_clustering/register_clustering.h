#ifndef REGISTER_CLUSTERING_H
#define REGISTER_CLUSTERING_H

#include "clusters.h"

namespace ophidian {
namespace register_clustering {
class register_clustering
{
public:
    using point = geometry::point<double>;
private:
    entity_system::entity_system m_clusters_system;
    clusters m_clusters;
public:
    register_clustering();
    ~register_clustering();

    entity_system::entity cluster_insert(point center);

    void cluster_clear(entity_system::entity cluster);

    void cluster_center(entity_system::entity cluster, point center);

    void flip_flop_insert(entity_system::entity cluster, clusters::cluster_element flip_flop);

    void flip_flop_insert(entity_system::entity cluster, std::vector<clusters::cluster_element>::iterator flip_flop_begin, std::vector<clusters::cluster_element>::iterator flip_flop_end);

    void flip_flop_reassign(clusters::cluster_element flip_flop, entity_system::entity cluster);

    const entity_system::entity_system & clusters_system() const {
        return m_clusters_system;
    }

    point cluster_center(entity_system::entity cluster) const {
        return m_clusters.center(cluster);
    }

    const std::vector<clusters::cluster_element> & cluster_flip_flops(entity_system::entity cluster) const {
        return m_clusters.flip_flops(cluster);
    }

    entity_system::entity flip_flop_cluster(entity_system::entity flip_flop) {
        return m_clusters.cluster(flip_flop);
    }

    const clusters & cluster_properties() const {
        return m_clusters;
    }
};
}
}



#endif // REGISTER_CLUSTERING_H
