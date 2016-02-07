#include "mysfmlcanvas.h"

#include <QDebug>
#include <random>

#include "resources.h"

mysfmlcanvas::mysfmlcanvas(QWidget *parent) :
    QSFMLCanvas(parent),
    m_app(nullptr),
    m_circuit(nullptr),
    m_view(sf::FloatRect(0,0,51599.25,34200.0)),
    m_state(new noninitialized),
    m_last_state(nullptr)
{
}

void mysfmlcanvas::app(application *app)
{
    m_app = app;
    m_circuit.reset(new circuit(app));
    delete m_state;
    m_state = new idle{*this};
}



void mysfmlcanvas::OnInit()
{

}

void mysfmlcanvas::OnUpdate()
{
    m_state = m_state->update();
    m_state->render(*this);

    m_fps.update();

}

#include <QWheelEvent>
void mysfmlcanvas::wheelEvent(QWheelEvent *e)
{
    if(e->delta() > 0)
        m_view.zoom(1.f/1.1f);
    else if(e->delta() < 0)
        m_view.zoom(1.1f);


    //    update_view_position();
    qDebug() << " view size " << m_view.getSize().x << " x " << m_view.getSize().y;
}

#include <QResizeEvent>
void mysfmlcanvas::resizeEvent(QResizeEvent *e)
{
    sf::RenderWindow::create((sf::WindowHandle) winId());
    //    QWidget::resizeEvent(e);
}

#include <QKeyEvent>
void mysfmlcanvas::update_view_position()
{

}

void mysfmlcanvas::drag_cell(openeda::entity::entity cell, point position)
{
    m_app->place_cell_and_update_index(cell, position);
    m_circuit->update_cell(cell);
}

openeda::entity::entity mysfmlcanvas::get_cell(point position)
{
    return m_app->get_cell(position);
}

point mysfmlcanvas::cell_position(openeda::entity::entity cell) const
{
    return m_app->placement().cell_position(cell);
}

void mysfmlcanvas::unselect()
{
    m_circuit->select_cell(openeda::entity::entity{});
}

void mysfmlcanvas::select(openeda::entity::entity cell)
{
    m_circuit->select_cell(cell);
}

void mysfmlcanvas::render_circuit()
{
    clear(sf::Color::White);
    setView(m_view);
    draw(*m_circuit);
    setView(getDefaultView());

    std::string fps_text{std::to_string(m_fps.getFPS())};
    fps_text = "FPS: " + fps_text;
    sf::Text fps{sf::String{fps_text.c_str()}, resources::font()};
    fps.setColor(sf::Color::Red);
    fps.setPosition(30.f,30.f);

    draw(fps);

}

void mysfmlcanvas::loading(bool set)
{
    if(set)
    {
        m_last_state = m_state;
        m_state = new loading_state{};
    } else {
        delete m_state;
        m_state = m_last_state;
    }
}

void mysfmlcanvas::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key::Key_W:
    case Qt::Key::Key_Up:
        m_view.move(sf::Vector2f(0.0f, -m_view.getSize().y*.1));
        break;
    case Qt::Key::Key_S:
    case Qt::Key::Key_Down:
        m_view.move(sf::Vector2f(0.0f, m_view.getSize().y*.1));
        break;
    case Qt::Key::Key_D:
    case Qt::Key::Key_Right:
        m_view.move(sf::Vector2f(m_view.getSize().x*.1, 0.0f));
        break;
    case Qt::Key::Key_A:
    case Qt::Key::Key_Left:
        m_view.move(sf::Vector2f(-m_view.getSize().x*.1, 0.0f));
        break;
    }

    //    update_view_position();

}

void mysfmlcanvas::mousePressEvent(QMouseEvent *e)
{
    sf::Vector2i pixelCoord{e->pos().x(), e->pos().y()};
    sf::Vector2f viewCoord{mapPixelToCoords(pixelCoord, m_view)};
    m_state = m_state->click(viewCoord.x, viewCoord.y);
}

void mysfmlcanvas::mouseMoveEvent(QMouseEvent *e)
{
    sf::Vector2i pixelCoord{e->pos().x(), e->pos().y()};
    sf::Vector2f viewCoord{mapPixelToCoords(pixelCoord, m_view)};
    m_state = m_state->mouse_move(viewCoord.x, viewCoord.y);
}

void mysfmlcanvas::mouseReleaseEvent(QMouseEvent *e)
{
    m_state = m_state->release();
}


idle::idle(mysfmlcanvas & canvas) :
    m_canvas(canvas)
{
    qDebug()<< "idle";

    canvas.unselect();

}

canvas_state *idle::click(double x, double y)
{
    delete this;
    return new clicking(m_canvas, x, y);
}


canvas_state *idle::update()
{
    return this;
}

clicking::clicking(mysfmlcanvas &canvas, double x, double y) :
    m_canvas(canvas),
    m_x(x),
    m_y(y)
{
    qDebug()<< "clicking " << x << ", " << y;
}


canvas_state *clicking::update()
{
    openeda::entity::entity selected = m_canvas.get_cell({m_x, -m_y});
    canvas_state * next = this;

    if(selected == openeda::entity::entity{})
    {
        delete this;
        next = new idle{m_canvas};
    }
    else
    {
        delete this;
        auto position = m_canvas.cell_position(selected);
        double offsetx{m_x - position.x()};
        double offsety{-m_y - position.y()};
        next = new selected_holding{m_canvas, selected, offsetx, offsety};
    }

    return next;
}


noninitialized::noninitialized()
{
    qDebug() <<"noninitialized";
}


void noninitialized::render(mysfmlcanvas &canvas)
{
    canvas.clear(sf::Color::Black);

    sf::Text pleaseload(sf::String("Please, open a circuit (Ctrl+O)."), resources::font());
    pleaseload.setColor(sf::Color::White);

    pleaseload.setPosition((canvas.getSize().x-pleaseload.getLocalBounds().width)/2.f, (canvas.getSize().y-pleaseload.getLocalBounds().height)/2.f);
    canvas.setView(canvas.getDefaultView());

    canvas.draw(pleaseload);
}


canvas_state::~canvas_state()
{

}

canvas_state *canvas_state::click(double x, double y)
{
    return this;
}

canvas_state *canvas_state::mouse_move(double x, double y)
{
    return this;
}

canvas_state *canvas_state::release()
{
    return this;
}

canvas_state *canvas_state::update()
{
    return this;
}

void canvas_state::render(mysfmlcanvas& canvas)
{
    canvas.render_circuit();
}


selected_holding::selected_holding(mysfmlcanvas &canvas, openeda::entity::entity cell, double x, double y):
    m_canvas(canvas),
    m_cell(cell),
    m_xoffset(x),
    m_yoffset(y)
{
    canvas.select(cell);
    qDebug() << "selected_holding offset " << x << ", " << y;
}

canvas_state *selected_holding::mouse_move(double x, double y)
{
    delete this;
    return new dragging{m_canvas, m_cell, m_xoffset, m_yoffset};
}

canvas_state *selected_holding::release()
{
    delete this;
    return new selected{m_canvas, m_cell};
}


selected::selected(mysfmlcanvas &canvas, openeda::entity::entity cell) :
    m_canvas(canvas),
    m_cell(cell)
{
    qDebug() << "selected ";
//    canvas.select(cell);
}


canvas_state *selected::click(double x, double y)
{
    delete this;
    return new clicking{m_canvas, x, y};
}


dragging::dragging(mysfmlcanvas &canvas, openeda::entity::entity cell, double x, double y) :
    m_canvas(canvas),
    m_cell(cell),
    m_xoffset(x),
    m_yoffset(y)
{
    qDebug() << "dragging offset " << x << ", " << y;
}


canvas_state *dragging::release()
{
    delete this;
    return new selected{m_canvas, m_cell};
}

canvas_state *dragging::mouse_move(double x, double y)
{
    m_canvas.drag_cell(m_cell, {x-m_xoffset, -y-m_yoffset});
    return this;
}


loading_state::loading_state()
{
    qDebug() <<"loading...";
}

void loading_state::render(mysfmlcanvas &canvas)
{
    canvas.clear(sf::Color::Black);

    sf::Text pleaseload(sf::String{"Please, wait. Loading..."}, resources::font());
    pleaseload.setColor(sf::Color::White);

    pleaseload.setPosition((canvas.getSize().x-pleaseload.getLocalBounds().width)/2.f, (canvas.getSize().y-pleaseload.getLocalBounds().height)/2.f);
    canvas.setView(canvas.getDefaultView());

    canvas.draw(pleaseload);
}
