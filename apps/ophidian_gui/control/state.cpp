#include "state.h"

State::State(MySFMLCanvas * SFMLCanvas, MainController * controller) :
    mSFMLCanvas(SFMLCanvas),
    mMainController(controller)
{

}

State::~State()
{

}

void State::update()
{

}

void State::State::wheelEvent(QWheelEvent * e)
{

}

void State::keyPressEvent(QKeyEvent * e)
{

}

void State::mousePressEvent(ophidian::geometry::Point pos)
{

}

void State::mouseMoveEvent(ophidian::geometry::Point pos)
{

}

void State::mouseReleaseEvent(ophidian::geometry::Point pos)
{

}

Blocked::Blocked(MySFMLCanvas * SFMLCanvas, MainController * controller) :
    State(SFMLCanvas, controller)
{

}

Blocked::~Blocked()
{

}

Idle::Idle(MySFMLCanvas * SFMLCanvas, MainController * controller) :
    State(SFMLCanvas, controller)
{

}

#include "view/mysfmlcanvas.h" //< Include needed here for MySFMLCanvas see the idle constructor without circular including

void Idle::mousePressEvent(ophidian::geometry::Point pos)
{
    mMainController->mousePress(pos);

    if(mMainController->hasQuad(pos))
    {
        Quad selected = mMainController->quadAt(pos);

        if(mMainController->isFixed(selected.mCell))
            mSFMLCanvas->setState(new Selected(mSFMLCanvas, mMainController, selected));
        else
            mSFMLCanvas->setState(new Dragging(mSFMLCanvas, mMainController, selected, pos));

        delete this;
    }
}

Selected::Selected(MySFMLCanvas * SFMLCanvas, MainController * controller, Quad quad) :
    State(SFMLCanvas, controller),
    mQuad(quad),
    mWireQuad(controller->wireQuadOfCell(quad.mCell))
{

}

Selected::~Selected()
{

}

void Selected::mousePressEvent(ophidian::geometry::Point pos)
{
    if(mMainController->hasQuad(pos))
    {
        Quad selected = mMainController->quadAt(pos);

        if (mQuad != selected)
        {
            mMainController->clear(mWireQuad);
            mMainController->mousePress(pos);

            if(mMainController->isFixed(selected.mCell))
                mSFMLCanvas->setState(new Selected(mSFMLCanvas, mMainController, selected));
            else
                mSFMLCanvas->setState(new Dragging(mSFMLCanvas, mMainController, selected, pos));
        }
        else
        {
            mMainController->clear(mWireQuad);
            mSFMLCanvas->setState(new Dragging(mSFMLCanvas, mMainController, selected, pos));
        }

    } else {
        mMainController->clear(mWireQuad);
        mMainController->mousePress(pos);
        mSFMLCanvas->setState(new Idle(mSFMLCanvas, mMainController));
    }

    delete this;
}

void Selected::keyPressEvent(QKeyEvent * e)
{

}

Dragging::Dragging(MySFMLCanvas * SFMLCanvas, MainController * controller, Quad quad, const ophidian::geometry::Point & pos) :
    State(SFMLCanvas, controller),
    mQuad(quad),
    mWireQuad(controller->wireQuadOfCell(quad.mCell)),
    mInitialPos(pos),
    mMoved(false)
{

}

void Dragging::mouseMoveEvent(ophidian::geometry::Point pos)
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
    mMainController->mouseMove(ophidian::geometry::Point(newOrigin.position.x, newOrigin.position.y));
}

void Dragging::mouseReleaseEvent(ophidian::geometry::Point pos)
{
    //mouseMoveEvent(pos); maybe need update the position?
    mMainController->clear(mWireQuad);
    mMainController->update(mQuad);
    mSFMLCanvas->setState(new Selected(mSFMLCanvas, mMainController, mQuad));
    delete this;
}
