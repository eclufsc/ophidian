#ifndef MYSFMLCANVAS_H
#define MYSFMLCANVAS_H

#include <QCoreApplication>

#include "qsfmlcanvas.h"
#include "application.h"

#include "circuit.h"

#include <unordered_map>

#include "fps.h"

class MainWindow;
class mysfmlcanvas;
class canvas_state {
public:
    virtual ~canvas_state();
    virtual canvas_state* click(double x, double y);
    virtual canvas_state* mouse_move(double x, double y);
    virtual canvas_state* release();
    virtual canvas_state* update();
    virtual void render(mysfmlcanvas& canvas);

};


class mysfmlcanvas : public QSFMLCanvas
{
public:


    application* m_app;
    std::unique_ptr<circuit> m_circuit;

    sf::View m_view;

    FPS m_fps;
    canvas_state *m_state;
    canvas_state *m_last_state;

    MainWindow * m_main_window;

public:
    mysfmlcanvas(QWidget *parent = 0);

    void app(application* app);

    void OnInit();
    void OnUpdate();


    void wheelEvent(QWheelEvent * e);
    void resizeEvent(QResizeEvent * e);
    void keyPressEvent(QKeyEvent* e);
    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent * e);


    void update_view_position();

    void drag_cell(openeda::entity::entity cell, point position);
    openeda::entity::entity get_cell(point position);
    point cell_position(openeda::entity::entity  cell) const;

    void unselect();
    void select(openeda::entity::entity  cell);

    void render_circuit();

    void loading(bool set);

    void main_window(MainWindow * main_window);

    void update_cell(openeda::entity::entity cell);

    void load_circuit();

};

class noninitialized : public canvas_state {
public:
    noninitialized();
    void render(mysfmlcanvas& canvas);
};

class idle : public canvas_state {
    mysfmlcanvas & m_canvas;
public:
    idle(mysfmlcanvas & canvas);
    virtual canvas_state* click(double x, double y);
    virtual canvas_state* update();



};

class clicking : public canvas_state {
    mysfmlcanvas & m_canvas;
    double m_x, m_y;
public:
    clicking(mysfmlcanvas & canvas, double x, double y);
    virtual canvas_state* update();

};

class selected_holding : public canvas_state {
    mysfmlcanvas & m_canvas;
    openeda::entity::entity m_cell;
    double m_xoffset, m_yoffset;
public:
    selected_holding(mysfmlcanvas & canvas, openeda::entity::entity cell, double x, double y);
    virtual canvas_state* mouse_move(double x, double y);
    virtual canvas_state* release();

};

class selected : public canvas_state {
    mysfmlcanvas & m_canvas;
    openeda::entity::entity m_cell;
public:
    selected(mysfmlcanvas & canvas, openeda::entity::entity cell);
    virtual canvas_state* click(double x, double y);
};

class dragging : public canvas_state {
    mysfmlcanvas & m_canvas;
    openeda::entity::entity m_cell;
    double m_xoffset, m_yoffset;
public:
    dragging(mysfmlcanvas & canvas, openeda::entity::entity cell, double x, double y);
    virtual canvas_state* release();
    virtual canvas_state* mouse_move(double x, double y);
};


class loading_state : public canvas_state {
public:
    loading_state();
    void render(mysfmlcanvas& canvas);
};



#endif // MYSFMLCANVAS_H
