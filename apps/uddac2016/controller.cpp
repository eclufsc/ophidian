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
        m_canvas_controller->create_index(geometries);
        random_purple_cell_painter painter;
        m_canvas_controller->paint_quads(painter);
        return true;
    }
    return false;
}

void controller::animate_solution()
{
    std::vector< std::pair<entity::entity, geometry::multi_polygon<geometry::polygon<geometry::point<double> > > > > geometries = m_app.cells_geometries();
    ophidian::gui::drawable_batch<4> destination_quads = m_canvas_controller->quads();
    m_canvas_controller->update_quads(destination_quads, geometries);
    ophidian::gui::batch_animation * animation = new ophidian::gui::batch_animation(m_canvas_controller->quads(), 30);
    for(int i = 0; i < m_canvas_controller->quads().vertex_count(); ++i)
    {
        (*animation)[i].position.x =  destination_quads[i].position.x-m_canvas_controller->quads()[i].position.x;
        (*animation)[i].position.y = destination_quads[i].position.y-m_canvas_controller->quads()[i].position.y;
    }
    m_canvas_controller->animate_quads(animation);
    m_canvas_controller->create_index(geometries);
}

void controller::read_def(const std::string &DEF)
{
    m_app.read_def(DEF);
    animate_solution();
}

void controller::init_canvas_controller(canvas_controller *canvas)
{
    m_canvas_controller = canvas;
    canvas->main_controller(this);
}


void controller::quads_animate(gui::batch_animation *animation)
{
    m_canvas_controller->animate_quads(animation);
}

void controller::run_SA(const std::string & verilog_file)
{
    m_app.run_SA(verilog_file);
    animate_solution();
}

void controller::place_cell(const entity::entity &cell, const ophidian::geometry::point<double> &p)
{
    m_app.cell_position(cell, p);
}



}
