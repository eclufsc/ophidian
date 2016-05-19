#ifndef OPHIDIAN_APPS_UDDAC2016_CANVAS_H
#define OPHIDIAN_APPS_UDDAC2016_CANVAS_H

#include "../gui/circuit_canvas.h"
#include "cell_painter.h"

#include <unordered_set>
#include <unordered_map>

namespace uddac2016 {

class controller;

class canvas : public ophidian::gui::circuit_canvas
{
    controller * m_main_ctrl;

    std::unordered_map< ophidian::entity_system::entity, std::vector< ophidian::gui::quad > > m_cell2quads;
    std::unordered_map< ophidian::gui::quad, ophidian::entity_system::entity > m_quad2cell;

public:
    canvas(QWidget * parent = 0);
    void main_controller(controller * main_ctrl);
    void create_quads(const std::vector< std::pair<ophidian::entity_system::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > & geometries);
    void update_quads(ophidian::gui::drawable_batch<4> &batch, const std::vector< std::pair<ophidian::entity_system::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > & geometries);
    void paint_quads(cell_painter & painter);
    void reindex(const std::vector< std::pair<ophidian::entity_system::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > & geometries);

    void setFixed(ophidian::entity_system::entity cell);

    virtual void select(ophidian::gui::quad the_quad);
    virtual void unselect(ophidian::gui::quad the_quad);

    virtual void dropQuad(ophidian::gui::quad the_quad);


    void update_quad(ophidian::entity_system::entity cell, const ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > & geometry);

};

}

#endif // OPHIDIAN_APPS_UDDAC2016_CANVAS_H
