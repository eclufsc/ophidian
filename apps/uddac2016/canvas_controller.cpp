#include "canvas_controller.h"
#include "sfml/mysfmlcanvas.h"
#include "controller.h"

#include <QDebug>

namespace uddac2016
{

using namespace ophidian;

canvas_controller::canvas_controller(mysfmlcanvas *canvas) :
    m_canvas(canvas),
    m_state(new idle_state(this))
{

}

void canvas_controller::main_controller(controller *ctrl)
{
    m_main_controller = ctrl;
}

void canvas_controller::state(canvas_state *state)
{
    m_state = state;
}

void canvas_controller::create_quads(const std::vector<std::pair<entity::entity, geometry::multi_polygon<geometry::polygon<geometry::point<double> > > > > &geometries)
{
    sf::Transform mirror;
    mirror.scale(1.0, -1.0);
    for(auto & entity_pair : geometries)
    {
        std::vector<gui::quad>& entity_quads = m_entity2quads[entity_pair.first];
        for(auto & geometry : entity_pair.second)
            entity_quads.push_back(m_canvas->quad_create(geometry.outer()[0], geometry.outer()[1], geometry.outer()[2], geometry.outer()[3]));
        for(auto & quad : entity_quads)
            m_canvas->transform(quad, mirror);
    }

    init_index(geometries);
}

void canvas_controller::update_quads(ophidian::gui::drawable_batch<4> &quads, const std::vector<std::pair<entity::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > &geometries)
{
    sf::Transform mirror;
    mirror.scale(1.0, -1.0);
    for(auto & entity_pair : geometries)
    {
        std::vector<gui::quad>& entity_quads = m_entity2quads[entity_pair.first];
        std::size_t quad_id{0};
        for(auto & geometry : entity_pair.second)
        {
            m_canvas->quad_update(entity_quads[quad_id], geometry.outer()[0], geometry.outer()[1], geometry.outer()[2], geometry.outer()[3]);
            ++quad_id;
        }
        for(auto & quad : entity_quads)
            m_canvas->transform(quad, mirror);
    }
}

void canvas_controller::animate_quads(gui::batch_animation *animation)
{
    m_canvas->quads_animate(animation);
}


void canvas_controller::init_index(const std::vector<std::pair<entity::entity, geometry::multi_polygon<geometry::polygon<geometry::point<double> > > > > & geometries) {
    m_index.clear();
    std::vector<rtree_node> nodes;
    for(auto & entity_pair : geometries)
    {
        for(auto & geo : entity_pair.second)
        {
            box cellbox, mirrored;
            boost::geometry::envelope(geo, cellbox);
            geometry::scale(cellbox, geometry::point<double>(1.0, -1.0), mirrored);
            rtree_node cell_node(mirrored, entity_pair.first);
            nodes.push_back(cell_node);
        }
    }
    m_index.insert(nodes.begin(), nodes.end());
}

void canvas_controller::paint_quads(cell_painter &painter)
{
    for(auto & entity_pair : m_entity2quads)
    {
        auto color = painter.color(entity_pair.first);
        for(auto & quad : entity_pair.second)
            m_canvas->paint(quad, color);
    }
}

void canvas_controller::mouse_press(const geometry::point<double> &p)
{
    m_state->mouse_press(p);
}

void canvas_controller::mouse_move(const ophidian::geometry::point<double> &p)
{
    m_state->mouse_move(p);
}

void canvas_controller::mouse_release()
{
    m_state->mouse_release();
}


ophidian::gui::drawable_batch<4> &canvas_controller::quads()
{
    return m_canvas->quads();
}


}
