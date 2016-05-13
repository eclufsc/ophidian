#include "canvas.h"
#include "controller.h"

using namespace ophidian;

namespace uddac2016 {

canvas::canvas(QWidget *parent) :
    gui::circuit_canvas(parent)
{

}

void canvas::main_controller(controller *main_ctrl)
{
    m_main_ctrl = main_ctrl;
}

void canvas::create_quads(const std::vector<std::pair<entity::entity, geometry::multi_polygon<geometry::polygon<geometry::point<double> > > > > &geometries)
{
    m_quad2cell.clear();
    m_cell2quads.clear();
    sf::Transform mirror;
    mirror.scale(1.0, -1.0);
    for(auto & entity_pair : geometries)
    {
        entity::entity cell_entity = entity_pair.first;
        const geometry::multi_polygon<geometry::polygon<geometry::point<double> > > & cell_geometry = entity_pair.second;
        auto & cell_quads = m_cell2quads[cell_entity];
        for(const geometry::polygon<geometry::point<double> > & polygon : cell_geometry)
        {
            sf::Transform translate;
            translate.translate(-polygon.outer()[0].x()+m_camera.getCenter().x, -polygon.outer()[0].y()+(-m_camera.getCenter().y));
            translate.combine(mirror);
            auto quad = drawQuad(polygon.outer()[0],polygon.outer()[1],polygon.outer()[2],polygon.outer()[3]);
            transform(quad, translate);
            cell_quads.push_back(quad);
            m_quad2cell[quad] = cell_entity;
        }
    }
    reindex(geometries);
}

void canvas::update_quads(gui::drawable_batch<4> & batch, const std::vector<std::pair<entity::entity, geometry::multi_polygon<geometry::polygon<geometry::point<double> > > > > &geometries)
{
    sf::Transform mirror;
    mirror.scale(1.0, -1.0);
    for(auto & entity_pair : geometries)
    {
        entity::entity cell_entity = entity_pair.first;
        const geometry::multi_polygon<geometry::polygon<geometry::point<double> > > & cell_geometry = entity_pair.second;
        std::size_t quad_id = 0;
        auto & quad_vector = m_cell2quads[cell_entity];
        for(const geometry::polygon<geometry::point<double> > & polygon : cell_geometry)
        {
            auto & quad = quad_vector[quad_id];
            for(int i = 0; i < 4; ++i)
                batch.set_point(quad.entity, i, polygon.outer()[i]);
            batch.transform(quad.entity, mirror);
            ++quad_id;
        }
    }
}

void canvas::paint_quads(cell_painter &painter)
{
    for(auto & entity_pair : m_cell2quads)
    {
        sf::Color cell_color = painter.color(entity_pair.first);
        for(auto & quad : entity_pair.second)
            paint(quad, painter.color(entity_pair.first));
    }
}

void canvas::reindex(const std::vector<std::pair<entity::entity, geometry::multi_polygon<geometry::polygon<geometry::point<double> > > > > &geometries)
{
    m_index.clear();
    for(auto & entity_pair : geometries)
    {
        entity::entity cell_entity = entity_pair.first;
        const geometry::multi_polygon<geometry::polygon<geometry::point<double> > > & cell_geometry = entity_pair.second;
        std::size_t quad_id = 0;
        auto & quad_vector = m_cell2quads[cell_entity];
        for(const geometry::polygon<geometry::point<double> > & polygon : cell_geometry)
        {
            auto & quad = quad_vector[quad_id];

            geometry::box< geometry::point<double> > box, mirrored;
            geometry::envelope(polygon, box);
            geometry::scale(box, geometry::point<double> {1.0, -1.0}, mirrored);
            m_index.quad_create(quad, mirrored.min_corner(), mirrored.max_corner());
            ++quad_id;
        }
    }
}

void canvas::setFixed(entity::entity cell)
{
    std::vector< gui::quad > & quads = m_cell2quads.at(cell);
    for(gui::quad & quad : quads)
        gui::circuit_canvas::setFixed(quad);
}

void canvas::select(gui::quad the_quad)
{
    gui::circuit_canvas::select(the_quad);
    auto the_cell = m_quad2cell.at(the_quad);
    m_main_ctrl->select(the_cell);
}

void canvas::unselect(gui::quad the_quad)
{
    gui::circuit_canvas::unselect(the_quad);
    auto the_cell = m_quad2cell.at(the_quad);
    m_main_ctrl->unselect(the_cell);
}

}
