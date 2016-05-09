#ifndef CANVAS_STATE_H
#define CANVAS_STATE_H

#include "../gui/canvas.h"
#include "../geometry/geometry.h"
namespace uddac2016 {


class canvas_controller;
class canvas_state
{
protected:
    canvas_controller * m_controller;
public:
    canvas_state(canvas_controller* controller);
    virtual ~canvas_state();
    virtual void mouse_press(const ophidian::geometry::point<double> &p) = 0 ;
    virtual void mouse_release() = 0;
    virtual void mouse_move(const ophidian::geometry::point<double> &p) = 0 ;
};

class idle_state : public canvas_state {
public:
    idle_state(canvas_controller* controller);
    void mouse_press(const ophidian::geometry::point<double> &p);
    void mouse_release();
    void mouse_move(const ophidian::geometry::point<double> &p);
};

class dragging_state : public canvas_state {
    ophidian::geometry::point<double> m_initial;
    ophidian::gui::quad m_selected;
public:
    dragging_state(canvas_controller *controller,  ophidian::gui::quad selected, const ophidian::geometry::point<double> &p);
    void mouse_press(const ophidian::geometry::point<double> &p);
    void mouse_release();
    void mouse_move(const ophidian::geometry::point<double> &p);
};

class selected_state : public canvas_state {
public:
    selected_state(canvas_controller *controller);
    void mouse_press(const ophidian::geometry::point<double> &p);
    void mouse_release();
    void mouse_move(const ophidian::geometry::point<double> &p);
};

}

#endif // CANVAS_STATE_H
