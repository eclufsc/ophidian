#ifndef OPHIDIAN_APPS_UDDAC2016_CONTROLLER_H
#define OPHIDIAN_APPS_UDDAC2016_CONTROLLER_H

#include <string>
#include "application.h"
#include "canvas_controller.h"

#include "sfml/mysfmlcanvas.h"

namespace uddac2016
{

class MainWindow;
class controller
{
    MainWindow & m_mainwindow;
    application m_app;
    canvas_controller * m_canvas_controller; // lazy init
public:
    controller(MainWindow & mainwindow);
    bool read_lefdef(const std::string & LEF, const std::string & DEF);
    void read_def(const std::string & DEF);
    void init_canvas_controller(canvas_controller *canvas);


    void place_cell(const ophidian::entity::entity & cell, const ophidian::geometry::point<double> & p);
    void quads_animate(ophidian::gui::batch_animation * animation);

    std::string cell_name(const ophidian::entity::entity & cell) const {
        return m_app.cell_name(cell);
    }

    ophidian::geometry::point<double> cell_position(const ophidian::entity::entity & cell) const {
        return m_app.cell_position(cell);
    }

    ophidian::geometry::multi_polygon< ophidian::geometry::polygon< ophidian::geometry::point<double> > > cell_geometry(const ophidian::entity::entity & cell) const {
        return m_app.cell_geometry(cell);
    }


    void run_SA(const std::string &verilog_file);
    void animate_solution();
};


}

#endif // OPHIDIAN_APPS_UDDAC2016_CONTROLLER_H
