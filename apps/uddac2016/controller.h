#ifndef OPHIDIAN_APPS_UDDAC2016_CONTROLLER_H
#define OPHIDIAN_APPS_UDDAC2016_CONTROLLER_H

#include <string>
#include "application.h"

#include "canvas.h"

#include "../gui/circuit_canvas.h"

namespace uddac2016
{

struct net {
    ophidian::entity_system::entity the_net;
    std::vector< ophidian::gui::line > lines;
};


class MainWindow;
class controller
{
    MainWindow & m_mainwindow;
    application m_app;
    uddac2016::canvas * m_canvas; // lazy init


    std::vector<net> m_nets;
    std::unordered_set<ophidian::entity_system::entity> m_visible_nets;

    std::vector<ophidian::entity_system::entity> nets_matching(const QString & regex);
public:
    controller(MainWindow & mainwindow);
    bool read_lefdef(const std::string & LEF, const std::string & DEF);
    void read_def(const std::string & DEF);
    void read_verilog(const std::string & v);


    void read_tau2014_lib(const std::string & file);
    void read_liberty(const std::string & file);


    void init_canvas_controller(uddac2016::canvas *canvas);


    void repaint();


    std::size_t show_nets(const QString & regex);
    void remove_nets(const QString & regex);
    void show_net(const ophidian::entity_system::entity & net);


    void place_cell(const ophidian::entity_system::entity & cell, const ophidian::geometry::point<double> & p);

    QString cell_name(const ophidian::entity_system::entity & cell) const {
        std::string cell_name = m_app.cell_name(cell);
        QString name(cell_name.c_str());
        return name;
    }

    QString cell_std_cell(const ophidian::entity_system::entity & cell) const {
        std::string std_cell = m_app.std_cell_name(m_app.cell_std_cell(cell));
        QString cell_std_cell(std_cell.c_str());
        return cell_std_cell;
    }

    void cell_std_cell(const ophidian::entity_system::entity &cell, QString std_cell)
    {
        auto std_cell_entity = m_app.std_cell_find(std_cell.toStdString());
        m_app.cell_std_cell(cell, std_cell_entity);
        auto cell_geometry = m_app.cell_geometry(cell);
        m_canvas->update_quad(cell, cell_geometry);
    }

    QStringList cell_std_cells(const ophidian::entity_system::entity & cell) const {
        QStringList result;
        for(auto std_cell : m_app.std_cells().cell_system())
            if(m_app.std_cell_pin_count(std_cell) == m_app.std_cell_pin_count(m_app.cell_std_cell(cell)))
                result.append(QString::fromStdString(m_app.std_cell_name(std_cell)));
        return result;
    }

    ophidian::geometry::point<double> cell_position(const ophidian::entity_system::entity & cell) const {
        return m_app.cell_position(cell);
    }

    ophidian::geometry::multi_polygon< ophidian::geometry::polygon< ophidian::geometry::point<double> > > cell_geometry(const ophidian::entity_system::entity & cell) const {
        return m_app.cell_geometry(cell);
    }

    bool cell_is_fixed(const ophidian::entity_system::entity & cell) const
    {
        return m_app.cell_is_fixed(cell);
    }
    double cell_worst_slack(const ophidian::entity_system::entity & cell) const
    {
        return m_app.cell_worst_slack(cell);
    }

    void select(const ophidian::entity_system::entity & cell);
    void unselect(const ophidian::entity_system::entity & cell);

    void screenshot();


    void run_SA();
    void run_STA();
    void animate_solution(std::size_t duration = 30);
    void update_ckt_info();
};


}

#endif // OPHIDIAN_APPS_UDDAC2016_CONTROLLER_H
