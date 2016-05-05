#include "mysfmlcanvas.h"
#include <QDebug>

#include <boost/geometry/arithmetic/arithmetic.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "../parsing/def.h"
#include "../parsing/lef.h"

#include "../netlist/netlist.h"

#include "../placement/placement.h"
#include "../placement/def2placement.h"
#include "../placement/lef2library.h"

#include "../floorplan/floorplan.h"
#include "../floorplan/lefdef2floorplan.h"

#include <QKeyEvent>
#include <QWheelEvent>

#include <omp.h>
#include <memory>



#include <QResizeEvent>

namespace uddac2016 {
using namespace ophidian;

mysfmlcanvas::mysfmlcanvas(QWidget *parent) :
    QSFMLCanvas(parent),
    m_camera_view(sf::FloatRect(0,0,51599.25,34200.0))
{
    m_opensans.loadFromFile("/usr/share/fonts/truetype/open-sans-elementary/OpenSans-Regular.ttf");
    omp_set_num_threads(omp_get_max_threads());
    m_camera_view.setViewport(sf::FloatRect(0.0, 0.0, 1.0, 1.0));
    //    auto quad = m_canvas.quad_create(geometry::point<double>(0.0, 0.0),
    //                         geometry::point<double>(0.0, 100.0),
    //                         geometry::point<double>(200.0, 100.0),
    //                         geometry::point<double>(200.0, 0.0));
    //    m_canvas.paint(quad, sf::Color::Green);

    std::unique_ptr<parsing::def> def;
    std::unique_ptr<parsing::lef> lef;
#pragma omp single nowait
    {
#pragma omp task shared(def)
        def.reset(new parsing::def("./benchmarks/superblue10/superblue10.def"));
#pragma omp task shared(lef)
        lef.reset(new parsing::lef("./benchmarks/superblue10/superblue10.lef"));
    }
#pragma omp taskwait

    standard_cell::standard_cells std_cells;
    netlist::netlist netlist(&std_cells);
    placement::library lib(&std_cells);
    placement::placement placement(&netlist, &lib);

    floorplan::floorplan fplan;

#pragma omp single nowait
    {
#pragma omp task shared(lef, def, placement, lib)
        {
            placement::def2placement(*def, placement);
            placement::lef2library(*lef, lib);
        }
#pragma omp task shared(lef, def, fplan)
        floorplan::lefdef2floorplan(*lef, *def, fplan);
    }
#pragma omp taskwait

    std::normal_distribution<double> blue_distribution(125.0, 24.0);
    std::default_random_engine engine;

    sf::Transform mirror;
    mirror.scale(1.0, -1.0);

    for(auto cell : netlist.cell_system())
    {
        auto geo = placement.cell_geometry(cell.first);
        for(auto & poly : geo)
        {

            auto quad = m_canvas.quad_create(poly.outer()[0], poly.outer()[1], poly.outer()[2], poly.outer()[3]);
            // mirroring y axis.
            m_canvas.transform(quad, mirror);
            // painting random blue
            double blue_tone{std::max(0.0, std::min(255.0, blue_distribution(engine)))};
            m_canvas.paint(quad, sf::Color(0,0,static_cast<sf::Uint8>(blue_tone)));
        }
    }
    m_camera_view.setSize(fplan.chip_boundaries().x()-fplan.chip_origin().x(), fplan.chip_boundaries().y()-fplan.chip_origin().y());
    m_camera_view.setCenter(fplan.chip_origin().x()+m_camera_view.getSize().x/2, -(fplan.chip_origin().y()+m_camera_view.getSize().y/2));


    auto line1 = m_canvas.line_create(fplan.chip_origin(), {fplan.chip_origin().x(), fplan.chip_boundaries().y()});
    auto line2 = m_canvas.line_create({fplan.chip_origin().x(), fplan.chip_boundaries().y()}, fplan.chip_boundaries());
    auto line3 = m_canvas.line_create(fplan.chip_boundaries(), {fplan.chip_boundaries().x(), fplan.chip_origin().y()});
    auto line4 = m_canvas.line_create({fplan.chip_boundaries().x(), fplan.chip_origin().y()}, fplan.chip_origin());

    m_canvas.transform(line1, mirror);
    m_canvas.transform(line2, mirror);
    m_canvas.transform(line3, mirror);
    m_canvas.transform(line4, mirror);
}

void mysfmlcanvas::resizeEvent(QResizeEvent *e)
{
    sf::RenderWindow::create((sf::WindowHandle) winId());
}

mysfmlcanvas::~mysfmlcanvas()
{
}


void mysfmlcanvas::OnInit()
{
}


void mysfmlcanvas::OnUpdate()
{
    static boost::posix_time::ptime last;
    boost::posix_time::ptime current = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration diff = current - last;
    clear(sf::Color::White);
    setView(m_camera_view);
    draw(m_canvas);
    setView(getDefaultView());
    std::string stdstring{std::to_string(1000./diff.total_milliseconds())};
    stdstring = "FPS: " + stdstring;
    sf::Text fps_text;
    fps_text.setString(sf::String(stdstring));
    fps_text.setPosition(30, 30);
    fps_text.setColor(sf::Color::Red);
    fps_text.setFont(m_opensans);
    draw(fps_text);
    last = current;
}

void mysfmlcanvas::wheelEvent(QWheelEvent *e)
{
    if(e->delta() > 0)
        m_camera_view.zoom(1.f/1.1f);
    else if(e->delta() < 0)
        m_camera_view.zoom(1.1f);
}
void mysfmlcanvas::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key::Key_W:
    case Qt::Key::Key_Up:
        m_camera_view.move(sf::Vector2f(0.0f, -m_camera_view.getSize().y*.1));
        break;
    case Qt::Key::Key_S:
    case Qt::Key::Key_Down:
        m_camera_view.move(sf::Vector2f(0.0f, m_camera_view.getSize().y*.1));
        break;
    case Qt::Key::Key_D:
    case Qt::Key::Key_Right:
        m_camera_view.move(sf::Vector2f(m_camera_view.getSize().x*.1, 0.0f));
        break;
    case Qt::Key::Key_A:
    case Qt::Key::Key_Left:
        m_camera_view.move(sf::Vector2f(-m_camera_view.getSize().x*.1, 0.0f));
        break;
    }
}

}
