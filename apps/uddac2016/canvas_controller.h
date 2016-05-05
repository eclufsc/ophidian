#ifndef CANVAS_CONTROLLER_H
#define CANVAS_CONTROLLER_H

#include "../gui/canvas.h"
#include "../entity/entity.h"
#include "../geometry/geometry.h"

#include "cell_painter.h"

#include "application.h"
#include "sfml/mysfmlcanvas.h"

namespace uddac2016
{


class canvas_controller
{
    mysfmlcanvas * m_canvas;
    std::unordered_map<ophidian::entity::entity, std::vector<ophidian::gui::quad> > m_entity2quads;

public:
    canvas_controller(mysfmlcanvas * canvas);
    void create_quads(const std::vector< std::pair<ophidian::entity::entity, ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > > > & geometries);
    void paint_quads(cell_painter & painter);

};

}

#endif // CANVAS_CONTROLLER_H
