#include "state.h"


State::State(MySFMLCanvas * SFMLCanvas, MainController * controller) :
    mSFMLCanvas(SFMLCanvas),
    mMainController(controller)
{

}

State::~State()
{

}

void State::wheelEvent(QWheelEvent * e)
{
    /* Should I perform this operation here or in MySFMLCanvas?
    if(e->delta() > 0)
        m_canvas->zoom(1.f/1.1f);
    else if(e->delta() < 0)
        m_canvas->zoom(1.1f);
    */
}

void State::keyPressEvent(QKeyEvent * e)
{
    /* Should I perform this operation here or in MySFMLCanvas?
    switch(e->key())
    {
    case Qt::Key::Key_W:
    case Qt::Key::Key_Up:
        m_canvas->moveCamera(sf::Vector2f(0.0f, -m_canvas->cameraSize().y*.1));
        break;
    case Qt::Key::Key_S:
    case Qt::Key::Key_Down:
        m_canvas->moveCamera(sf::Vector2f(0.0f, m_canvas->cameraSize().y*.1));
        break;
    case Qt::Key::Key_D:
    case Qt::Key::Key_Right:
        m_canvas->moveCamera(sf::Vector2f(m_canvas->cameraSize().x*.1, 0.0f));
        break;
    case Qt::Key::Key_A:
    case Qt::Key::Key_Left:
        m_canvas->moveCamera(sf::Vector2f(-m_canvas->cameraSize().x*.1, 0.0f));
        break;
    }
    */
}

Blocked::Blocked(MySFMLCanvas * SFMLCanvas, MainController * controller) :
    State(SFMLCanvas, controller)
{

}

Idle::Idle(MySFMLCanvas * SFMLCanvas, MainController * controller) :
    State(SFMLCanvas, controller)
{

}

void Idle::mousePressEvent(QMouseEvent * e)
{
    ophidian::geometry::Point pixelCoord(e->pos().x(), e->pos().y());
    // ??? ophidian::geometry::Point viewCoord{m_canvas->mapPixelToCamera(pixelCoord)};
    if(mMainController->hasQuad(pixelCoord))
    {
        //if(m_canvas->isFixed(m_canvas->quadAt(viewCoord)))
            //mSFMLCanvas->state(new selected(mSFMLCanvas, mMainController, mMainController->quadAt(pixelCoord)));
        //else
        //    m_canvas->state(new dragging(m_canvas, viewCoord));
        //delete this;
    }
}

Selected::Selected(MySFMLCanvas * SFMLCanvas, MainController * controller, Quad quad) :
    State(SFMLCanvas, controller),
    mQuad(quad)
{

}

Selected::~Selected()
{

}

void Selected::mousePressEvent(QMouseEvent * e)
{

}

void Selected::keyPressEvent(QKeyEvent * e)
{

}

Dragging::Dragging(MySFMLCanvas * SFMLCanvas, MainController * controller, const ophidian::geometry::Point & pos) :
    State(SFMLCanvas, controller),
    mInitialPosition(pos)
{

}

void Dragging::mouseMoveEvent(QMouseEvent * e)
{

}

void Dragging::mouseReleaseEvent(QMouseEvent * e)
{

}
