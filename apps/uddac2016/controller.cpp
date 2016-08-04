#include "controller.h"

#include "mainwindow.h"

#include <fstream>
#include <functional>

#include "random_purple_cell_painter.h"

#include <QDebug>
#include <QDateTime>

#include "../interconnection/flute.h"

#include <QRegularExpression>

using namespace ophidian;

namespace uddac2016
{

std::vector<entity_system::entity> controller::nets_matching(const QString &regex )
{
    std::vector<entity_system::entity> result;
    QRegularExpression expression(regex);
    for(auto net : m_app.nets())
    {
        std::string net_name = m_app.net_name(net);
        auto match = expression.match(QString::fromStdString(net_name));
        if(match.hasMatch())
            result.push_back(net);
    }
    return result;
}

controller::controller(MainWindow &mainwindow) :
    m_mainwindow(mainwindow)
{

}

void controller::update_ckt_info()
{
    m_mainwindow.cell_count(m_app.cell_count());
    m_mainwindow.pin_count(m_app.pin_count());
    m_mainwindow.net_count(m_app.net_count());
    m_mainwindow.die_area(QPoint(m_app.chip_boundaries()[3].x()/m_app.placement_units2micron(), m_app.chip_boundaries()[1].y()/m_app.placement_units2micron()));
    m_mainwindow.circuit_name(QString::fromStdString(m_app.circuit_name()));
}

void controller::show_cells(bool show)
{
    m_canvas->showCells(show);
}

bool controller::read_lefdef(const std::string &LEF, const std::string &DEF)
{
    std::ifstream lef(LEF.c_str());
    std::ifstream def(DEF.c_str());
    if(!lef.good() || !def.good())
        return false;
    if(m_app.read_lefdef(LEF, DEF))
    {
        m_mainwindow.reset();

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

        update_ckt_info();

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

void controller::read_verilog(const std::string &v)
{
    if(m_app.read_verilog(v))
        update_ckt_info();
}

void controller::read_ops(const std::string &ops)
{
    m_app.read_ops(ops);
}

void controller::read_tau2014_lib(const std::string &file)
{
    m_app.read_tau2014_lib(file);
}

void controller::read_liberty(const std::string &file)
{
    m_app.read_liberty(file);
}

void controller::init_canvas_controller(uddac2016::canvas *canvas)
{
    m_canvas = canvas;
    canvas->main_controller(this);

}

void controller::repaint()
{
    if(m_app.STA())
    {
        auto worst_slacks = m_app.worst_slacks();
        range_painter painter;
        QColor negative0 = m_mainwindow.negativeSlackBegin();
        QColor negativeF = m_mainwindow.negativeSlackEnd();
        QColor zero = m_mainwindow.zeroSlack();
        QColor positive = m_mainwindow.positiveSlack();

        painter.negative(std::make_pair(
                             sf::Color(negative0.red(), negative0.green(), negative0.blue()),
                             sf::Color(negativeF.red(), negativeF.green(), negativeF.blue())
                             ));
        painter.zero(sf::Color(zero.red(), zero.green(), zero.blue()));
        painter.positive(sf::Color(positive.red(), positive.green(), positive.blue()));
        painter.color_map(worst_slacks);
        m_canvas->paint_quads(painter);
    }
}

std::size_t controller::show_nets(const QString &regex)
{
    auto matching = nets_matching(regex);
    for(auto net : matching)
        show_net(net);
    qDebug() << matching.size() << " matches";
}

void controller::remove_nets(const QString &regex)
{
    auto matching = nets_matching(regex);
    qDebug() << "remove nets matching " << regex << " (" <<  matching.size() << ")";


    for(const net & n : m_nets)
    {
        if(std::find(matching.begin(), matching.end(), n.the_net) != matching.end())
        {

            for(auto & line : n.lines)
                m_canvas->erase(line);
        }
    }

    m_nets.erase( std::remove_if(m_nets.begin(), m_nets.end(), [matching](const net& n)->bool{
        return std::find(matching.begin(), matching.end(), n.the_net) != matching.end();
    }), m_nets.end() );

    for(auto entity : matching)
        m_visible_nets.erase(entity);
}

void controller::show_net(const entity_system::entity &net)
{
    if(!m_visible_nets.insert(net).second) return;
    uddac2016::net the_net;
    the_net.the_net = net;

    auto net_pins = m_app.net_pins(net);
    std::vector<unsigned> X(net_pins.size()), Y(net_pins.size());
    X.resize(0);
    Y.resize(0);
    for(auto pin : net_pins)
    {
        auto pos = m_app.pin_position(pin);
        X.push_back(static_cast<unsigned>(pos.x()));
        Y.push_back(static_cast<unsigned>(pos.y()));
    }
    auto tree = interconnection::flute(X.size(), X.data(), Y.data(), ACCURACY);
    std::size_t num_branches = 2 * tree.deg - 2;

    the_net.lines.reserve(2*num_branches);

    for(std::size_t i = 0; i < num_branches; ++i)
    {
        if(i == tree.branch[i].n) continue;
        geometry::point<double> p1(tree.branch[i].x, tree.branch[i].y);
        geometry::point<double> p2(tree.branch[tree.branch[i].n].x, tree.branch[i].y);
        geometry::point<double> p3(tree.branch[tree.branch[i].n].x, tree.branch[tree.branch[i].n].y);

        p1.y(-p1.y());
        p2.y(-p2.y());
        p3.y(-p3.y());

        the_net.lines.push_back(m_canvas->drawLine(p1, p2));
        the_net.lines.push_back(m_canvas->drawLine(p2, p3));
    }
    m_nets.push_back(the_net);
    delete[] tree.branch;
}

void controller::run_SA()
{
    for(int i = 0; i < 10; ++i)
        m_app.run_SA();

    //    std::cout << "will legalize " << std::endl;
    //    m_app.legalize();
    //    std::cout << "legalize DONE" << std::endl;

    animate_solution();
}

void controller::run_STA()
{
    m_app.run_STA();
    repaint();
    if(m_mainwindow.isCriticalPathChecked())
        show_CP();
}

void controller::place_cell(const entity_system::entity &cell, const ophidian::geometry::point<double> &p)
{
    m_app.cell_position(cell, p);
    m_canvas->update_quad(cell, m_app.cell_geometry(cell));
}

void controller::unselect(const entity_system::entity &cell)
{
    m_mainwindow.unselect();
}

void controller::screenshot()
{
    QDateTime date = QDateTime::currentDateTime();
    const std::string filename = "uddac2016-" + date.toString().toStdString() + ".png";
    m_canvas->save_to_file(filename);
}

void controller::update_visible_nets()
{
    m_mainwindow.update_visible_nets();
}

void controller::show_CP()
{
    qDebug() << "controller::show_CP()";
    auto cp = m_app.critical_path();
    m_canvas->create_cp_arrows(cp);
}

void controller::clear_CP()
{
    qDebug() << "controller::clear_CP()";
    m_canvas->destroy_cp_arrows();
}

void controller::select(const entity_system::entity &cell)
{
    qDebug() << QString::fromStdString(m_app.cell_name(cell));
    m_mainwindow.select(cell);
}

}
