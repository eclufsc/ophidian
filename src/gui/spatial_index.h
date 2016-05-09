#ifndef SPATIAL_INDEX_H
#define SPATIAL_INDEX_H

#include "canvas.h"

#include <boost/geometry/index/rtree.hpp>

#include "../geometry/geometry.h"


namespace ophidian {
namespace gui {

typedef std::pair<geometry::box<geometry::point<double> >, quad> rtree_node;
class rtree_node_comparator {
public:
    bool operator()(const rtree_node & node1, const rtree_node & node2) const {
        return node1.second == node2.second;
    }
};
typedef boost::geometry::index::rtree<rtree_node,
boost::geometry::index::rstar<16>,
boost::geometry::index::indexable<rtree_node>, rtree_node_comparator> rtree;


class spatial_index
{
    canvas & m_canvas;
    rtree m_index;
public:
    spatial_index(canvas & can);

    void quad_create(quad the_quad);

    void quad_create(quad the_quad, const geometry::point<double> & p1, const geometry::point<double> & p2);


    std::vector<quad> quads_containing(const geometry::point<double> & p1) const {
        std::vector<rtree_node> result;
        m_index.query(boost::geometry::index::contains(p1),
                      std::back_inserter(result));
        std::vector<quad> the_quads(result.size());
        for(std::size_t i = 0; i < result.size(); ++i)
            the_quads[i] = result[i].second;

        return the_quads;
    }
    void quad_remove(quad the_quad);

    void clear();

};

}
}

#endif // SPATIAL_INDEX_H
