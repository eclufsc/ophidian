#include "register_clustering.h"

namespace ophidian {
namespace register_clustering {
register_clustering::register_clustering()
    : m_clusters(m_clusters_system) {

}

register_clustering::~register_clustering()
{

}

entity_system::entity register_clustering::cluster_insert(register_clustering::point center)
{
    auto cluster = m_clusters_system.create();
    m_clusters.center(cluster, center);
    return cluster;
}

void register_clustering::cluster_clear(entity_system::entity cluster)
{
    m_clusters.remove_flip_flops(cluster);
}

void register_clustering::flip_flop_insert(entity_system::entity cluster, clusters::cluster_element flip_flop)
{
    m_clusters.insert_flip_flop(cluster, flip_flop);
}

void register_clustering::flip_flop_insert(entity_system::entity cluster, std::vector<clusters::cluster_element>::iterator flip_flop_begin, std::vector<clusters::cluster_element>::iterator flip_flop_end)
{
    for (auto flip_flop_it = flip_flop_begin; flip_flop_it != flip_flop_end; ++flip_flop_it) {
        flip_flop_insert(cluster, *flip_flop_it);
    }
}

void register_clustering::flip_flop_reassign(clusters::cluster_element flip_flop, entity_system::entity cluster)
{
    m_clusters.remove_flip_flop(m_clusters.cluster(flip_flop.first), flip_flop);
    m_clusters.insert_flip_flop(cluster, flip_flop);
}

void register_clustering::cluster_center(entity_system::entity cluster, register_clustering::point center)
{
    m_clusters.center(cluster, center);
}

}
}


