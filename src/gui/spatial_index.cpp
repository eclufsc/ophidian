#include "spatial_index.h"

#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>


namespace ophidian {
namespace gui {

spatial_index::spatial_index(canvas &can):
    m_canvas(can)
{
}

void spatial_index::quad_create(quad the_quad)
{
    const std::array<sf::Vertex, 4> & points = m_canvas.quad_points(the_quad);
    geometry::point<double> max(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest());
    geometry::point<double> min(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());

    for(auto point : points )
    {
        max.x( std::max( max.x(), static_cast<double>(point.position.x) ));
        max.y( std::max( max.y(), static_cast<double>(point.position.y) ));
        min.x( std::min( min.x(), static_cast<double>(point.position.x) ));
        min.y( std::min( min.y(), static_cast<double>(point.position.y) ));
    }
    geometry::box<geometry::point<double> > bounds(min, max);
    m_index.insert(rtree_node(bounds, the_quad));
}

void spatial_index::quad_create(quad the_quad, const geometry::point<double> &p1, const geometry::point<double> &p2)
{
    geometry::box<geometry::point<double> > bounds(p1, p2);
    m_index.insert(rtree_node(bounds, the_quad));
}

void spatial_index::quad_remove(quad the_quad)
{
    std::vector<rtree_node> to_remove;
    m_index.query(boost::geometry::index::covered_by(m_index.bounds()), std::back_inserter(to_remove));
    auto begin = std::remove_if(to_remove.begin(), to_remove.end(), [the_quad](const rtree_node & node)->bool{
        return !(node.second == the_quad);
    });
    to_remove.erase(begin, to_remove.end());
    m_index.remove(to_remove.begin(), to_remove.end());
}

void spatial_index::clear()
{
    m_index.clear();
}

}
}
