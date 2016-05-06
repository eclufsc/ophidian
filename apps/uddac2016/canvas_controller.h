#ifndef CANVAS_CONTROLLER_H
#define CANVAS_CONTROLLER_H

#include "../gui/canvas.h"
#include "../entity/entity.h"
#include "../geometry/geometry.h"

#include "cell_painter.h"

#include "application.h"

#include <boost/geometry/index/rtree.hpp>

#include "canvas_state.h"

namespace uddac2016
{

using box = typename boost::geometry::model::box<ophidian::geometry::point<double> >;
typedef std::pair<box, ophidian::entity::entity> rtree_node;
class rtree_node_comparator {
public:
    bool operator()(const rtree_node & node1, const rtree_node & node2) const {
        return node1.second == node2.second;
    }
};

typedef boost::geometry::index::rtree<rtree_node,
boost::geometry::index::rstar<16>,
boost::geometry::index::indexable<rtree_node>, rtree_node_comparator> rtree;

class mysfmlcanvas;
class controller;
class canvas_controller
{
    controller * m_main_controller;
    mysfmlcanvas * m_canvas;
    canvas_state * m_state;
    std::unordered_map<ophidian::entity::entity, std::vector<ophidian::gui::quad> > m_entity2quads;
    rtree m_index;
    void init_index(const std::vector<std::pair<ophidian::entity::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > &geometries);
public:
    canvas_controller(mysfmlcanvas * canvas);
    void main_controller(controller * ctrl);
    void state(canvas_state * state);

    void create_quads(const std::vector< std::pair<ophidian::entity::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > & geometries);
    void update_quads(ophidian::gui::drawable_batch<4> & quads, const std::vector< std::pair<ophidian::entity::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > & geometries);

    void animate_quads(ophidian::gui::batch_animation * animation);

    void paint_quads(cell_painter & painter);


    void mouse_press(const ophidian::geometry::point<double> & p);
    void mouse_move(const ophidian::geometry::point<double> & p);
    void mouse_release();

    ophidian::gui::drawable_batch<4> & quads();



    std::vector<ophidian::entity::entity> cells_at(const ophidian::geometry::point<double> & p) const {
        std::vector<rtree_node> result;
        m_index.query(boost::geometry::index::contains(p),
                                    std::back_inserter(result));
        std::vector<ophidian::entity::entity> the_return(result.size());
        for(int i = 0; i < result.size(); ++i)
            the_return[i] = result[i].second;
        return the_return;
    }

};

}

#endif // CANVAS_CONTROLLER_H
