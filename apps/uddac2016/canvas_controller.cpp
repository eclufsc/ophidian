#include "canvas_controller.h"

namespace uddac2016
{

canvas_controller::canvas_controller(mysfmlcanvas *canvas) :
    m_canvas(canvas)
{
}

void canvas_controller::create_quads(const std::vector<std::pair<ophidian::entity::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > &geometries)
{
    sf::Transform mirror;
    mirror.scale(1.0, -1.0);
    for(auto & entity_pair : geometries)
    {
        std::vector<ophidian::gui::quad>& entity_quads = m_entity2quads[entity_pair.first];
        for(auto & geometry : entity_pair.second)
            entity_quads.push_back(m_canvas->quad_create(geometry.outer()[0], geometry.outer()[1], geometry.outer()[2], geometry.outer()[3]));
        for(auto & quad : entity_quads)
            m_canvas->transform(quad, mirror);
    }
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


}
