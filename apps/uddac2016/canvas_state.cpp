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
    qDebug() << "idle_state::idle_state()";
}

void idle_state::mouse_press(const ophidian::geometry::point<double> &p)
{
    qDebug() << "idle_state::mouse_press(" << p.x() << ", " << p.y() << ")";

    auto quads = m_controller->quads_at(p);
    if(!quads.empty())
    {
        m_controller->state(new dragging_state(m_controller, quads.front(), p));
        delete this;
    } else
        qDebug() <<  "no cell at position";
}

void idle_state::mouse_release()
{
    qDebug() << "idle_state::mouse_release()";

}

void idle_state::mouse_move(const ophidian::geometry::point<double> &p)
{
    qDebug() << "idle_state::mouse_move(" << p.x() << ", " << p.y() << ")";

}

dragging_state::dragging_state(canvas_controller *controller, ophidian::gui::quad selected, const ophidian::geometry::point<double> &p) :
    canvas_state(controller),
    m_initial(p),
    m_selected(selected)
{
    qDebug() << "dragging_state::dragging_state()";
}

void dragging_state::mouse_press(const ophidian::geometry::point<double> &p)
{
    qDebug() << "dragging_state::mouse_press(" << p.x() << ", " << p.y() << ")";

}

void dragging_state::mouse_release()
{
    qDebug() << "dragging_state::mouse_release()";
    m_controller->commit_quad_position(m_selected);
    m_controller->state(new selected_state(m_controller));
    delete this;
}

void dragging_state::mouse_move(const ophidian::geometry::point<double> &p)
{
    qDebug() << "dragging_state::mouse_move(" << p.x() << ", " << p.y() << ")";
    const ophidian::geometry::point<double> delta(p.x()-m_initial.x(), p.y()-m_initial.y());
    m_controller->move_quad(m_selected, delta);
    m_initial = p;
}

selected_state::selected_state(canvas_controller *controller) :
    canvas_state(controller)
{
    qDebug() << "selected_state::selected_state()";
}

void selected_state::mouse_press(const ophidian::geometry::point<double> &p)
{
    qDebug() << "selected_state::mouse_press(" << p.x() << ", " << p.y() << ")";
    auto quads = m_controller->quads_at(p);
    if(!quads.empty())
        m_controller->state(new dragging_state(m_controller, quads.front(), p));
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
