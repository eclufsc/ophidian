#ifndef STATE_H
#define STATE_H

#include <QWheelEvent>
#include <QKeyEvent>

#include <ophidian/geometry/Models.h>
#include "control/maincontroller.h"
#include "Model/forms.h"

class State
{
public:
    State(MainController * controller);
    ~State();
    virtual void update(State * & state);
    virtual void wheelEvent(QWheelEvent * e, State * & state);
    virtual void keyPressEvent(QKeyEvent * e, State * & state);
    virtual void mousePressEvent(ophidian::geometry::Point pos, State * & state);
    virtual void mouseMoveEvent(ophidian::geometry::Point pos, State * & state);
    virtual void mouseReleaseEvent(ophidian::geometry::Point pos, State * & state);

protected:
    MainController * mMainController;
};

class Blocked : public State
{
public:
    Blocked(MainController * controller);
    ~Blocked();
};

class Idle : public State
{
public:
    Idle(MainController * controller);
    void mousePressEvent(ophidian::geometry::Point pos, State * & state);
};

class Selected : public State
{
public:
    Selected(MainController * controller, Quad quad);
    ~Selected();
    void mousePressEvent(ophidian::geometry::Point pos, State * & state);
    void keyPressEvent(QKeyEvent * e, State * & state);

protected:
    WireQuad mWireQuad;
    Quad mQuad;
};

class Dragging : public State
{
public:
    Dragging(MainController * controller, Quad quad, const ophidian::geometry::Point & pos);
    void mouseMoveEvent(ophidian::geometry::Point pos, State * & state);
    void mouseReleaseEvent(ophidian::geometry::Point pos, State * & state);

protected:
    Quad mQuad;
    ophidian::geometry::Point mInitialPos;
    bool mMoved;
};

#endif // STATE_H
