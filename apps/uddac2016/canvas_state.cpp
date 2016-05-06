#include "canvas_state.h"
#include "canvas_controller.h"

#include <QDebug>

namespace uddac2016 {

canvas_state::canvas_state(canvas_controller *controller) :
    m_controller(controller)
{

}

canvas_state::~canvas_state()
{

}

idle_state::idle_state(canvas_controller *controller) :
    canvas_state(controller)
{

}

void idle_state::mouse_press(const ophidian::geometry::point<double> &p)
{
    qDebug() << "idle_state::mouse_press(" << p.x() << ", " << p.y() << ")";
    auto cells = m_controller->cells_at(p);
    if(!cells.empty())
    {
        m_controller->state(new dragging_state(m_controller));
        delete this;
    }
}

void idle_state::mouse_release()
{
    qDebug() << "idle_state::mouse_release()";

}

void idle_state::mouse_move(const ophidian::geometry::point<double> &p)
{
    qDebug() << "idle_state::mouse_move(" << p.x() << ", " << p.y() << ")";

}

dragging_state::dragging_state(canvas_controller *controller) :
    canvas_state(controller)
{

}

void dragging_state::mouse_press(const ophidian::geometry::point<double> &p)
{
    qDebug() << "dragging_state::mouse_press(" << p.x() << ", " << p.y() << ")";

}

void dragging_state::mouse_release()
{
    qDebug() << "dragging_state::mouse_release()";
    m_controller->state(new selected_state(m_controller));
    delete this;
}

void dragging_state::mouse_move(const ophidian::geometry::point<double> &p)
{
    qDebug() << "dragging_state::mouse_move(" << p.x() << ", " << p.y() << ")";
}

selected_state::selected_state(canvas_controller *controller) :
    canvas_state(controller)
{

}

void selected_state::mouse_press(const ophidian::geometry::point<double> &p)
{
    qDebug() << "selected_state::mouse_press(" << p.x() << ", " << p.y() << ")";
    auto cells = m_controller->cells_at(p);
    if(!cells.empty())
        m_controller->state(new dragging_state(m_controller));
    else
        m_controller->state(new idle_state(m_controller));
    delete this;
}

void selected_state::mouse_release()
{
    qDebug() << "selected_state::mouse_release()";

}

void selected_state::mouse_move(const ophidian::geometry::point<double> &p)
{
    qDebug() << "selected_state::mouse_move(" << p.x() << ", " << p.y() << ")";

}

}
