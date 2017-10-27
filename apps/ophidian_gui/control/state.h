#ifndef STATE_H
#define STATE_H

#include <ophidian/geometry/Models.h>

#include "view/mysfmlcanvas.h"
#include "control/maincontroller.h"
#include "Model/forms.h"

class State
{
public:
    State(MySFMLCanvas * SFMLCanvas, MainController * controller);
    virtual ~State();
    virtual void update() = 0;
    virtual void wheelEvent(QWheelEvent * e);
    virtual void keyPressEvent(QKeyEvent * e);
    virtual void mousePressEvent(QMouseEvent * e) = 0;
    virtual void mouseMoveEvent(QMouseEvent * e) = 0;
    virtual void mouseReleaseEvent(QMouseEvent * e) = 0;

protected:
    MySFMLCanvas * mSFMLCanvas;
    MainController * mMainController;
};

class Blocked : public State
{
public:
    Blocked(MySFMLCanvas * SFMLCanvas, MainController * controller);
    virtual void update() = 0;
    virtual void wheelEvent(QWheelEvent * e) = 0;
    virtual void keyPressEvent(QKeyEvent * e) = 0;
    virtual void mousePressEvent(QMouseEvent * e) = 0;
    virtual void mouseMoveEvent(QMouseEvent *e) = 0;
    virtual void mouseReleaseEvent(QMouseEvent * e) = 0;
};

class Idle : public State
{
public:
    Idle(MySFMLCanvas * SFMLCanvas, MainController * controller);
    void mousePressEvent(QMouseEvent * e);
};

class Selected : public State
{
    //wire_quad m_wirequad;
    Quad mQuad;
public:
    Selected(MySFMLCanvas * SFMLCanvas, MainController * controller, Quad quad);
    virtual ~Selected();
    void mousePressEvent(QMouseEvent * e);
    void keyPressEvent(QKeyEvent * e);
};

class Dragging : public State
{
    Quad mQuad;
    ophidian::geometry::Point mInitialPosition;
    bool mMoved;
public:
    Dragging(MySFMLCanvas * SFMLCanvas, MainController * controller, const ophidian::geometry::Point & pos);
    void mouseMoveEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent * e);
};

#endif // STATE_H
