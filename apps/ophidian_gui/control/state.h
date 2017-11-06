#ifndef STATE_H
#define STATE_H

#include <QWheelEvent>
#include <QKeyEvent>

#include <ophidian/geometry/Models.h>
#include "control/maincontroller.h"
#include "Model/forms.h"

//#include "view/mysfmlcanvas.h"

class MySFMLCanvas;

class State
{
public:
    State(MySFMLCanvas * SFMLCanvas, MainController * controller);
    ~State();
    virtual void update();
    virtual void wheelEvent(QWheelEvent * e);
    virtual void keyPressEvent(QKeyEvent * e);
    virtual void mousePressEvent(ophidian::geometry::Point pos);
    virtual void mouseMoveEvent(ophidian::geometry::Point pos);
    virtual void mouseReleaseEvent(ophidian::geometry::Point pos);

protected:
    MySFMLCanvas * mSFMLCanvas;
    MainController * mMainController;
};

class Blocked : public State
{
public:
    Blocked(MySFMLCanvas * SFMLCanvas, MainController * controller);
    ~Blocked();
};

class Idle : public State
{
public:
    Idle(MySFMLCanvas * SFMLCanvas, MainController * controller);
    void mousePressEvent(ophidian::geometry::Point pos);
};

class Selected : public State
{
public:
    Selected(MySFMLCanvas * SFMLCanvas, MainController * controller, Quad quad);
    ~Selected();
    void mousePressEvent(ophidian::geometry::Point pos);
    void mouseReleaseEvent(ophidian::geometry::Point pos);
    void keyPressEvent(QKeyEvent * e);

protected:
    Quad mQuad;
    WireQuad mWireQuad;
};

class Dragging : public State
{
public:
    Dragging(MySFMLCanvas * SFMLCanvas, MainController * controller, Quad quad, const ophidian::geometry::Point & pos);
    void mouseMoveEvent(ophidian::geometry::Point pos);
    void mouseReleaseEvent(ophidian::geometry::Point pos);

protected:
    Quad mQuad;
    WireQuad mWireQuad;
    ophidian::geometry::Point mInitialPos;
    bool mMoved;
};

#endif // STATE_H
