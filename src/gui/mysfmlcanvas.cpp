#include "mysfmlcanvas.h"
#include <QDebug>

#include <boost/geometry/arithmetic/arithmetic.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <QKeyEvent>
#include <QWheelEvent>

#include <omp.h>
#include <memory>

#include "../geometry/geometry.h"

#include <QResizeEvent>

namespace uddac2016 {
using namespace ophidian;

mysfmlcanvas::mysfmlcanvas(QWidget *parent) :
    m_controller(this),
    QSFMLCanvas(parent),
    m_camera_view(sf::FloatRect(0,0,51599.25,34200.0))
{
    m_opensans.loadFromFile("/usr/share/fonts/truetype/open-sans-elementary/OpenSans-Regular.ttf");
    omp_set_num_threads(omp_get_max_threads());
    m_camera_view.setViewport(sf::FloatRect(0.0, 0.0, 1.0, 1.0));

}

void mysfmlcanvas::resizeEvent(QResizeEvent *e)
{
    sf::RenderWindow::create((sf::WindowHandle) winId());
}

mysfmlcanvas::~mysfmlcanvas()
{
}

canvas_controller *mysfmlcanvas::controller()
{
    return &m_controller;
}

gui::canvas *mysfmlcanvas::canvas()
{
    return &m_canvas;
}


void mysfmlcanvas::OnInit()
{
}


void mysfmlcanvas::OnUpdate()
{
    m_canvas.update();
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

void mysfmlcanvas::mousePressEvent(QMouseEvent *e)
{
    sf::Vector2i pixelCoord{e->pos().x(), e->pos().y()};
    sf::Vector2f viewCoord{mapPixelToCoords(pixelCoord, m_camera_view)};
    m_controller.mouse_press(geometry::point<double>(viewCoord.x, viewCoord.y));
}

void mysfmlcanvas::mouseMoveEvent(QMouseEvent *e)
{
    sf::Vector2i pixelCoord{e->pos().x(), e->pos().y()};
    sf::Vector2f viewCoord{mapPixelToCoords(pixelCoord, m_camera_view)};
    m_controller.mouse_move(geometry::point<double>(viewCoord.x, viewCoord.y));
}

void mysfmlcanvas::mouseReleaseEvent(QMouseEvent *e)
{
    m_controller.mouse_release();
}

void mysfmlcanvas::center_view_on(const ophidian::geometry::point<double> &p1)
{
    m_camera_view.setCenter(sf::Vector2f(p1.x(), p1.y()));
}

void mysfmlcanvas::view_size(const ophidian::geometry::point<double> &size)
{
    m_camera_view.setSize(sf::Vector2f(size.x(), size.y()));
}

gui::line mysfmlcanvas::line_create(const ophidian::geometry::point<double> &p1, const ophidian::geometry::point<double> &p2)
{
    return m_canvas.line_create(p1, p2);
}


gui::quad mysfmlcanvas::quad_create(const ophidian::geometry::point<double> &p1, const ophidian::geometry::point<double> &p2, const ophidian::geometry::point<double> &p3, const ophidian::geometry::point<double> &p4)
{
    return m_canvas.quad_create(p1, p2, p3, p4);
}

void mysfmlcanvas::quads_animate(gui::batch_animation *animation)
{
    m_canvas.quads_animate(animation);

}

ophidian::gui::drawable_batch<4> &mysfmlcanvas::quads()
{
    return m_canvas.quads();
}


}
