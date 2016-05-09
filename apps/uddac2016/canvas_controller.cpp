#include "canvas_controller.h"
#include "sfml/mysfmlcanvas.h"
#include "controller.h"

#include <QDebug>

namespace uddac2016
{

using namespace ophidian;

canvas_controller::canvas_controller(mysfmlcanvas *canvas) :
    m_canvas(canvas),
    m_state(new idle_state(this)),
    m_index(*canvas->canvas())
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

void canvas_controller::create_index(const std::vector<std::pair<entity::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > &geometries)
{
    m_index.clear();
    sf::Transform mirror;
    mirror.scale(1.0, -1.0);
    for(auto & entity_pair : geometries)
    {
        std::vector<gui::quad>& entity_quads = m_entity2quads[entity_pair.first];
        std::size_t quad_id = 0;
        if(entity_pair.second.size() > 1) continue;
        for(auto & geo : entity_pair.second)
        {
            geometry::box< geometry::point<double> > envelope, mirrored;
            geometry::envelope(geo, envelope);
            geometry::scale(envelope, geometry::point<double>(1.0, -1.0), mirrored);
            m_index.quad_create(entity_quads[quad_id], mirrored.min_corner(), mirrored.max_corner());
            ++quad_id;
        }
    }
}

void canvas_controller::create_quads(const std::vector<std::pair<entity::entity, geometry::multi_polygon<geometry::polygon<geometry::point<double> > > > > &geometries)
{
    m_entity2quads.clear();
    m_quad2entity.clear();
    sf::Transform mirror;
    mirror.scale(1.0, -1.0);
    for(auto & entity_pair : geometries)
    {
        std::vector<gui::quad>& entity_quads = m_entity2quads[entity_pair.first];
        for(auto & geometry : entity_pair.second)
            entity_quads.push_back(m_canvas->quad_create(geometry.outer()[0], geometry.outer()[1], geometry.outer()[2], geometry.outer()[3]));
        for(auto & quad : entity_quads)
        {
            m_quad2entity[quad] = entity_pair.first;
            m_canvas->transform(quad, mirror);
        }
    }
}

void canvas_controller::update_quads(ophidian::gui::drawable_batch<4> &quads, const std::vector<std::pair<entity::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > &geometries)
{
    sf::Transform mirror;
    mirror.scale(1.0, -1.0);
    for(auto & entity_pair : geometries)
    {
        std::vector<gui::quad>& entity_quads = m_entity2quads[entity_pair.first];
        std::size_t quad_id{0};
        for(auto & the_geometry : entity_pair.second)
        {
            for(std::size_t point_id{0}; point_id < 4; ++point_id)
                quads.set_point(entity_quads[quad_id].entity, point_id, the_geometry.outer()[point_id]);
            quads.transform(entity_quads[quad_id].entity, mirror);
            ++quad_id;
        }
    }
}

void canvas_controller::animate_quads(gui::batch_animation *animation)
{
    m_canvas->quads_animate(animation);
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

void canvas_controller::commit_quad_position(gui::quad the_quad)
{
    m_index.quad_remove(the_quad);
    m_index.quad_create(the_quad);

    auto the_entity = m_quad2entity[the_quad];
    m_main_controller->place_cell(the_entity, ophidian::geometry::point<double>{m_canvas->quad_points(the_quad).front().position.x, -m_canvas->quad_points(the_quad).front().position.y});

}

const std::array<sf::Vertex, 4> &canvas_controller::quad_points(gui::quad the_quad) const
{
    return m_canvas->quad_points(the_quad);
}

void canvas_controller::move_quad(gui::quad the_quad, const ophidian::geometry::point<double> &delta)
{
    sf::Transform translate;
    translate.translate(delta.x(), delta.y());
    m_canvas->transform(the_quad, translate);
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
