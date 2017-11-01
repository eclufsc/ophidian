#include "state.h"


State::State(MainController * controller) :
    mMainController(controller)
{

}

State::~State()
{

}

void State::wheelEvent(QWheelEvent * e, State * & state)
{
    /* Should I perform this operation here or in MySFMLCanvas?
    if(e->delta() > 0)
        m_canvas->zoom(1.f/1.1f);
    else if(e->delta() < 0)
        m_canvas->zoom(1.1f);
    */
}

void State::keyPressEvent(QKeyEvent * e, State * & state)
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


void State::update(State * & state)
{

}

void State::mousePressEvent(ophidian::geometry::Point pos, State * & state)
{

}

void State::mouseMoveEvent(ophidian::geometry::Point pos, State * & state)
{

}

void State::mouseReleaseEvent(ophidian::geometry::Point pos, State * & state)
{

}

Blocked::Blocked(MainController * controller) :
    State(controller)
{

}

Blocked::~Blocked()
{

}

Idle::Idle(MainController * controller) :
    State(controller)
{

}

void Idle::mousePressEvent(ophidian::geometry::Point pos, State * & state)
{
    mMainController->mousePress(pos);

    if(mMainController->hasQuad(pos))
    {
        Quad selected = mMainController->quadAt(pos);

        if(mMainController->isFixed(selected.mCell))
            state = new Selected(mMainController, selected);
        else
            state = new Dragging(mMainController, selected, pos);

        delete this;
    }
}

Selected::Selected(MainController * controller, Quad quad) :
    State(controller),
    mQuad(quad),
    mWireQuad(controller->wireQuadOfCell(quad.mCell))
{

}

Selected::~Selected()
{

}

void Selected::mousePressEvent(ophidian::geometry::Point pos, State * & state)
{
    if(mMainController->hasQuad(pos))
    {
        Quad selected = mMainController->quadAt(pos);

        if (mQuad != selected)
        {
            mMainController->clear(mWireQuad);
            mMainController->mousePress(pos);

            if(mMainController->isFixed(selected.mCell))
                state = new Selected(mMainController, selected);
            else
                state = new Dragging(mMainController, selected, pos);
        }
        else
        {
            mMainController->clear(mWireQuad);
            state = new Dragging(mMainController, selected, pos);
        }

    } else {
        mMainController->clear(mWireQuad);
        mMainController->mousePress(pos);
        state = new Idle(mMainController);
    }

    delete this;
}

void Selected::keyPressEvent(QKeyEvent * e, State * & state)
{

}

Dragging::Dragging(MainController * controller, Quad quad, const ophidian::geometry::Point & pos) :
    State(controller),
    mQuad(quad),
    mWireQuad(controller->wireQuadOfCell(quad.mCell)),
    mInitialPos(pos),
    mMoved(false)
{

}

void Dragging::mouseMoveEvent(ophidian::geometry::Point pos, State * & state)
{
    sf::Vector2f delta(pos.x() - mInitialPos.x(), pos.y() - mInitialPos.y());
    sf::Transform translation;

    translation.translate(delta.x, delta.y);
    mInitialPos = pos;
    mMoved = true;

    mMainController->transform(mQuad, translation);
    mMainController->transform(mWireQuad, translation);

    Quad first = mMainController->quadsCell(mQuad.mCell).front();
    auto newOrigin = mMainController->getCanvas()->points(first).front();
    mMainController->mousePress(ophidian::geometry::Point(newOrigin.position.x, newOrigin.position.y));
}

void Dragging::mouseReleaseEvent(ophidian::geometry::Point pos, State * & state)
{
    mMainController->clear(mWireQuad);
    mMainController->update(mQuad);
    state = state = new Selected(mMainController, mQuad);
    delete this;
}
