#include "controller.h"

#include "mainwindow.h"

#include <fstream>

#include "random_purple_cell_painter.h"

using namespace ophidian;

namespace uddac2016
{

controller::controller(MainWindow &mainwindow) :
    m_mainwindow(mainwindow)
{

}

bool controller::read_lefdef(const std::string &LEF, const std::string &DEF)
{
    std::ifstream lef(LEF.c_str());
    std::ifstream def(DEF.c_str());
    if(!lef.good() || !def.good())
        return false;
    if(m_app.read_lefdef(LEF, DEF))
    {
        std::vector< std::pair<entity::entity, geometry::multi_polygon<geometry::polygon<geometry::point<double> > > > > geometries = m_app.cells_geometries();
        m_canvas_controller->create_quads(geometries);
        random_purple_cell_painter painter;
        m_canvas_controller->paint_quads(painter);
        return true;
    }
    return false;
}

void controller::init_canvas_controller(mysfmlcanvas * canvas)
{
    m_canvas_controller.reset(new canvas_controller(canvas));
}

}
