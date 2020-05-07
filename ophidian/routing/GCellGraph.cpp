#include <ophidian/routing/GCellGraph.h>

namespace ophidian::routing{


GCellGraph::GCellGraph(GCellGraph::unit_container_type x, GCellGraph::unit_container_type y, GCellGraph::index_type z, GCellGraph::scalar_container_type capacity):
    ophidian::util::GridGraph_3D(x.size()-1,y.size()-1,z), m_nodes_to_gcell(m_graph)
{
    for (index_type x_it = 0; x_it < x.size() -1; ++x_it) {
        for (index_type y_it = 0; y_it < y.size() -1; ++y_it) {
            for (index_type z_it = 0; z_it < z; ++z_it) {
                auto gcell = m_gcells.add();
                auto node_inst = node(x_it, y_it, z_it);
                m_gcell_node[gcell] = node_inst;
                m_nodes_to_gcell[node_inst] = gcell;
                if(z_it > capacity.size()-1 )
                {
                    //if capacity is not provided for this z the last capacity is replicated for this layer
                    m_gcell_capacity[gcell] = capacity[capacity.size()-1];
                }else{
                    m_gcell_capacity[gcell] = capacity[z_it];
                }
            }

            auto min_corner = point_type{x.at(x_it), y.at(y_it)}; 
            auto max_corner = point_type{x.at(x_it + 1), y.at(y_it + 1)};
            m_gcell_box.emplace(std::make_pair( std::make_pair(x_it,y_it) , box_type(min_corner, max_corner) ));

            //generate RTree;
            // using rtree_node_type       = std::pair<box_scalar_type, std::pair<index_type, index_type>>;
            // using rtree_type            = boost::geometry::index::rtree<rtree_node_type, boost::geometry::index::rstar<16> >;
            auto min_corner_d = point_scalar_type{units::unit_cast<double>(x.at(x_it)), units::unit_cast<double>(y.at(y_it))}; 
            auto max_corner_d = point_scalar_type{units::unit_cast<double>(x.at(x_it + 1)), units::unit_cast<double>(y.at(y_it + 1))};

            auto box = box_scalar_type{min_corner_d, max_corner_d};
            m_grid.insert(std::make_pair(box, std::make_pair(x_it,y_it)));
        }
    }
}

GCellGraph::GCellGraph(const ophidian::routing::Library & library, GCellGraph::unit_container_type x, GCellGraph::unit_container_type y, GCellGraph::index_type z):
    ophidian::util::GridGraph_3D(x.size()-1,y.size()-1,z), m_nodes_to_gcell(m_graph)
{
    for (index_type z_it = 0; z_it < z; ++z_it) {

        auto layer = library.layer_from_index(z_it+1);
        auto track = library.prefTrack(layer);
        auto tracDir = library.direction(track);
        auto numTracks = library.numTracs(track);
        auto start = units::unit_cast<double>(library.start(track));
        auto space = units::unit_cast<double>(library.space(track));
        double last_track_max_cord = start + (numTracks -1)*space;

        for (index_type x_it = 0; x_it < x.size() -1; ++x_it) {
            for (index_type y_it = 0; y_it < y.size() -1; ++y_it) {
                auto min_corner = point_type{x.at(x_it), y.at(y_it)}; 
                auto max_corner = point_type{x.at(x_it + 1), y.at(y_it + 1)};
                m_gcell_box.emplace(std::make_pair( std::make_pair(x_it,y_it) , box_type(min_corner, max_corner) ));
                
                auto gcell = m_gcells.add();
                auto node_inst = node(x_it, y_it, z_it);
                m_gcell_node[gcell] = node_inst;
                m_nodes_to_gcell[node_inst] = gcell;
                
                //cap = (max - start)/space  --  (min-start)/space
                int capacity = 0;
                double min_cord = 0;
                double max_cord = 0;
                if(tracDir == ophidian::routing::Direction::HORIZONTAL){
                    min_cord = units::unit_cast<double>(min_corner.y());
                    max_cord = units::unit_cast<double>(max_corner.y());
                }else if (tracDir == ophidian::routing::Direction::VERTICAL){
                    min_cord = units::unit_cast<double>(min_corner.x());
                    max_cord = units::unit_cast<double>(max_corner.x());
                }
                if(max_cord > last_track_max_cord)
                    max_cord = last_track_max_cord;
                capacity = (int) std::ceil((max_cord - start)/space) - std::ceil((min_cord - start)/space);

                if(capacity == 0 && max_cord == start)
                    capacity = 1;
                m_gcell_capacity[gcell] = capacity;
                
                if(z_it == 0){ // generate Rtree once
                    //generate RTree;
                    auto min_corner_d = point_scalar_type{units::unit_cast<double>(x.at(x_it)), units::unit_cast<double>(y.at(y_it))}; 
                    auto max_corner_d = point_scalar_type{units::unit_cast<double>(x.at(x_it + 1)), units::unit_cast<double>(y.at(y_it + 1))};
                    auto box = box_scalar_type{min_corner_d, max_corner_d};
                    m_grid.insert(std::make_pair(box, std::make_pair(x_it,y_it)));
                }
            }
        }
    }
}

GCellGraph::gcell_container_type::const_iterator GCellGraph::begin_gcell() const noexcept
{
    return m_gcells.begin();
}

GCellGraph::gcell_container_type::const_iterator GCellGraph::end_gcell() const noexcept
{
    return m_gcells.end();
}

GCellGraph::gcell_type GCellGraph::gcell(GCellGraph::index_type x, GCellGraph::index_type y, GCellGraph::index_type z) const
{
    auto n = node(x,y,z);
    if(n != lemon::INVALID)
        return m_nodes_to_gcell[n];
    return gcell_type();
}

GCellGraph::gcell_type GCellGraph::gcell(GCellGraph::node_type & node) const
{
    if(node != lemon::INVALID)
        return m_nodes_to_gcell[node];
    return gcell_type();
}

GCellGraph::gcell_type GCellGraph::nearest_gcell(const GCellGraph::point_type location, const GCellGraph::index_type layer) const
{
    namespace bgi = boost::geometry::index;
    point_scalar_type point{location.x().value(), location.y().value()};
    std::vector<rtree_node_type> result;
    m_grid.query(bgi::nearest(point, 1), std::back_inserter(result));
    auto node_index_pair = result.front().second;
    return gcell(node_index_pair.first, node_index_pair.second, layer);
}

GCellGraph::box_type GCellGraph::box(const GCellGraph::gcell_type& gcell)
{
    auto node = m_gcell_node[gcell];
    auto index = position(node);
    auto x = index.get<0>();
    auto y = index.get<1>();
    return m_gcell_box[std::make_pair(x,y)];
}

GCellGraph::point_type GCellGraph::center_of_box(const GCellGraph::gcell_type& gcell)
{
    box_type gbox = box(gcell);
    // std::pair<unit_type, unit_type> min{std::numeric_limits<unit_type>::max(), std::numeric_limits<unit_type>::max()};
    // std::pair<unit_type, unit_type> max{std::numeric_limits<unit_type>::min(), std::numeric_limits<unit_type>::min()};

    // min.first = std::min(gbox.min_corner().x(), gbox.max_corner().x());
    // min.second = std::min(gbox.min_corner().y(), gbox.max_corner().y());
    // max.first = std::max(gbox.min_corner().x(), gbox.max_corner().x());
    // max.second = std::max(gbox.min_corner().y(), gbox.max_corner().y());

    auto min_x = gbox.min_corner().x();
    auto min_y = gbox.min_corner().y();
    auto max_x = gbox.max_corner().x();
    auto max_y = gbox.max_corner().y();

    // return point_type{ min.first + ((max.first - min.first)/2), min.second + ((max.second - min.second) / 2) };
    return point_type{ (min_x+max_x)/2 , (min_y+max_y)/2 };
    // return point_type();
}

GCellGraph::node_type GCellGraph::graph_node(const GCellGraph::gcell_type gcell) const
{
    return m_gcell_node[gcell];
}

GCellGraph::scalar_type GCellGraph::capacity(const GCellGraph::gcell_type& gcell)
{
    return m_gcell_capacity[gcell];
}

void GCellGraph::capacity(const GCellGraph::gcell_type& gcell, scalar_type capacity)
{
    m_gcell_capacity[gcell] = capacity;
}

GCellGraph::scalar_type GCellGraph::demand(const GCellGraph::gcell_type& gcell)
{
    return m_gcell_blockage_demand[gcell] + m_gcell_net_demand[gcell];
}

bool GCellGraph::overfloed(const gcell_type& gcell){
    return (m_gcell_capacity[gcell] < (m_gcell_blockage_demand[gcell] + m_gcell_net_demand[gcell]));
}

GCellGraph::scalar_type GCellGraph::net_demand(const GCellGraph::gcell_type& gcell)
{
    return m_gcell_net_demand[gcell];
}

GCellGraph::scalar_type GCellGraph::blockage_demand(const GCellGraph::gcell_type& gcell)
{
    return m_gcell_blockage_demand[gcell];
}

void GCellGraph::change_net_demand(const GCellGraph::gcell_type& gcell, const scalar_type delta)
{
    m_gcell_net_demand[gcell] += delta;
}

void GCellGraph::change_blockage_demand(const GCellGraph::gcell_type& gcell, const scalar_type delta)
{
    m_gcell_blockage_demand[gcell] += delta;
}

void GCellGraph::intersect(GCellGraph::gcell_container_type& gcells, const GCellGraph::box_type box, const GCellGraph::index_type layer)
{
    namespace bgi = boost::geometry::index;
    auto min_corner_d = point_scalar_type{units::unit_cast<double>(box.min_corner().x()), units::unit_cast<double>(box.min_corner().y())}; 
    auto max_corner_d = point_scalar_type{units::unit_cast<double>(box.max_corner().x()), units::unit_cast<double>(box.max_corner().y())};
    auto box_d = box_scalar_type{min_corner_d, max_corner_d};

    // using rtree_node_type       = std::pair<box_scalar_type, std::pair<index_type, index_type>>;
    std::vector<rtree_node_type> result;
    m_grid.query(bgi::intersects(box_d), std::back_inserter(result));
    for(auto r : result)
    {
        auto node_index_pair = r.second;
        auto node_box = r.first;

        //box_scalar_type intersection;
        //boost::geometry::intersection(node_box, box_d, intersection);
        //if(boost::geometry::area(intersection) != 0)
            gcells.push_back(gcell(node_index_pair.first, node_index_pair.second, layer));
    }
}

uint32_t GCellGraph::id(const GCellGraph::gcell_type& gcell)
{
    return m_gcells.id(gcell);
}

entity_system::EntitySystem<GCellGraph::gcell_type>::NotifierType * GCellGraph::notifier_gcells() const noexcept {
    return m_gcells.notifier();
}

}
