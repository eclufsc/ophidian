#ifndef CANVAS_CONTROLLER_H
#define CANVAS_CONTROLLER_H

#include "../gui/canvas.h"
#include "../entity/entity.h"
#include "../geometry/geometry.h"

#include "cell_painter.h"
#include "application.h"


#include <boost/bimap/unordered_set_of.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>

namespace uddac2016
{


class mysfmlcanvas;
class controller;
class canvas_controller
{
    controller * m_main_controller;

    std::unordered_map<ophidian::entity::entity, std::vector<ophidian::gui::quad> > m_entity2quads;
    std::unordered_map<ophidian::gui::quad, ophidian::entity::entity> m_quad2entity;



public:
    canvas_controller(mysfmlcanvas * canvas);
    void reset_state();
    void main_controller(controller * ctrl);
    void state(canvas_state * state);

    void create_index(const std::vector< std::pair<ophidian::entity::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > & geometries);

    void create_quads(const std::vector< std::pair<ophidian::entity::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > & geometries);
    void update_quads(ophidian::gui::drawable_batch<4> & quads, const std::vector< std::pair<ophidian::entity::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > & geometries);
    void animate_quads(ophidian::gui::batch_animation * animation);
    void paint_quads(cell_painter & painter);


    ophidian::gui::wire_quad create_wire_quad(const ophidian::gui::quad & quad);
    void remove_wire_quad(const ophidian::gui::wire_quad &quad);
    void move_wire_quad(const ophidian::gui::wire_quad &quad, const ophidian::geometry::point<double> & delta);


    void commit_quad_position(ophidian::gui::quad the_quad);
    std::vector<ophidian::gui::quad> quads_at(const ophidian::geometry::point<double> & p) const {
        return m_index.quads_containing(p);
    }

    const std::array<sf::Vertex, 4> & quad_points(ophidian::gui::quad the_quad) const;

    void move_quad(ophidian::gui::quad the_quad, const ophidian::geometry::point<double> & delta);
    void mouse_press(const ophidian::geometry::point<double> & p);
    void mouse_move(const ophidian::geometry::point<double> & p);
    void mouse_release();
    ophidian::gui::drawable_batch<4> & quads();
};

}

#endif // CANVAS_CONTROLLER_H
