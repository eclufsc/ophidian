#include "controller.h"

#include "mainwindow.h"

#include <fstream>
#include <functional>

#include "random_purple_cell_painter.h"

#include <QDebug>

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
        sf::Vector2f new_center((m_app.chip_boundaries()[0].x()+m_app.chip_boundaries()[3].x())/2.0, -(m_app.chip_boundaries()[0].y()+m_app.chip_boundaries()[1].y())/2.0);
        m_canvas->cameraCenter(new_center);
        m_canvas->cameraSize(sf::Vector2f(m_app.chip_boundaries()[3].x()-m_app.chip_boundaries()[0].x(), m_app.chip_boundaries()[1].y()-m_app.chip_boundaries()[0].y()));
        std::vector< std::pair<entity_system::entity, geometry::multi_polygon<geometry::polygon<geometry::point<double> > > > > geometries = m_app.cells_geometries();
        m_canvas->create_quads(geometries);
        random_purple_cell_painter painter(std::bind(&application::cell_is_fixed, &m_app, std::placeholders::_1));
        m_canvas->paint_quads(painter);
        for(auto & pair : geometries)
        {
            if(m_app.cell_is_fixed(pair.first))
                m_canvas->setFixed(pair.first);
        }
        auto boundaries = m_app.chip_boundaries();
        auto quad = m_canvas->drawRect(boundaries[0],boundaries[1],boundaries[2],boundaries[3]);
        sf::Transform mirror;
        mirror.scale(1.0, -1.0);
        for(std::size_t i = 0; i < 4; ++i)
            m_canvas->transform(quad.lines[i], mirror);


        animate_solution(30);

        return true;
    }
    return false;
}

void controller::animate_solution(std::size_t duration)
{
    std::vector< std::pair<entity_system::entity, geometry::multi_polygon<geometry::polygon<geometry::point<double> > > > > geometries = m_app.cells_geometries();
    ophidian::gui::drawable_batch<4> destination_quads = m_canvas->quadsBatch();
    m_canvas->update_quads(destination_quads, geometries);
    ophidian::gui::batch_animation * animation = new ophidian::gui::batch_animation(m_canvas->quadsBatch(), duration);
    for(int i = 0; i < m_canvas->quadsBatch().vertex_count(); ++i)
    {
        (*animation)[i].position.x =  destination_quads[i].position.x-m_canvas->quadsBatch()[i].position.x;
        (*animation)[i].position.y = destination_quads[i].position.y-m_canvas->quadsBatch()[i].position.y;
    }
    m_canvas->animate(animation);
    m_canvas->reindex(geometries);
}

void controller::read_def(const std::string &DEF)
{
    m_app.read_def(DEF);
    animate_solution();
    m_canvas->reset();
}

void controller::init_canvas_controller(uddac2016::canvas *canvas)
{
    m_canvas = canvas;
    canvas->main_controller(this);
}

void controller::run_SA(const std::string & verilog_file)
{
    m_app.run_SA(verilog_file);
    animate_solution();
}

void controller::place_cell(const entity_system::entity &cell, const ophidian::geometry::point<double> &p)
{
    m_app.cell_position(cell, p);
}

void controller::unselect(const entity_system::entity &cell)
{

}

void controller::select(const entity_system::entity &cell)
{
    qDebug() << QString::fromStdString(m_app.cell_name(cell));
}



}
