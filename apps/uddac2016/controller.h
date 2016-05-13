#ifndef OPHIDIAN_APPS_UDDAC2016_CONTROLLER_H
#define OPHIDIAN_APPS_UDDAC2016_CONTROLLER_H

#include <string>
#include "application.h"

#include "canvas.h"

#include "../gui/circuit_canvas.h"

namespace uddac2016
{

class MainWindow;
class controller
{
    MainWindow & m_mainwindow;
    application m_app;
    uddac2016::canvas * m_canvas; // lazy init
public:
    controller(MainWindow & mainwindow);
    bool read_lefdef(const std::string & LEF, const std::string & DEF);
    void read_def(const std::string & DEF);
    void init_canvas_controller(uddac2016::canvas *canvas);


    void place_cell(const ophidian::entity_system::entity & cell, const ophidian::geometry::point<double> & p);

    std::string cell_name(const ophidian::entity_system::entity & cell) const {
        return m_app.cell_name(cell);
    }

    ophidian::geometry::point<double> cell_position(const ophidian::entity_system::entity & cell) const {
        return m_app.cell_position(cell);
    }

    ophidian::geometry::multi_polygon< ophidian::geometry::polygon< ophidian::geometry::point<double> > > cell_geometry(const ophidian::entity_system::entity & cell) const {
        return m_app.cell_geometry(cell);
    }

    void select(const ophidian::entity_system::entity & cell);
    void unselect(const ophidian::entity_system::entity & cell);


    void run_SA(const std::string &verilog_file);
    void animate_solution(std::size_t duration = 30);
};


}

#endif // OPHIDIAN_APPS_UDDAC2016_CONTROLLER_H
