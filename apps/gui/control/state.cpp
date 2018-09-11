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

void State::mousePressEvent(State::point_type pos)
{

}

void State::mouseMoveEvent(State::point_type pos)
{

}

void State::mouseReleaseEvent(State::point_type pos)
{

}

bool State::findCellEvent(QString name)
{
    return false;
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

//! This include is needed here for MySFMLCanvas see the idle constructor without circular including
#include "view/mysfmlcanvas.h"

void Idle::mousePressEvent(State::point_type pos)
{
    mMainController->mousePress(pos);

    if(mMainController->hasQuad(pos))
    {
        Quad selected = mMainController->quadAt(pos);

        if(mMainController->isFixed(selected.mCell)) {
            mSFMLCanvas->setState(new Selected(mSFMLCanvas, mMainController, selected));
        } else {
            mSFMLCanvas->setState(new Dragging(mSFMLCanvas, mMainController, selected, pos));
        }

        delete this;
    }
}

bool Idle::findCellEvent(QString name)
{
    if(mMainController->hasCell(name.toStdString()))
    {
        Quad selected = mMainController->quadsCell(name.toStdString()).front();

        mMainController->selectedCell(selected.mCell);

        auto origin = mSFMLCanvas->canvas()->points(selected).front();
        auto size = mMainController->cellSize(selected.mCell);
        mSFMLCanvas->centerViewOn(State::point_type(origin.position.x + (size.x() / 2), origin.position.y + (size.y() / 2)));

        mSFMLCanvas->setState(new Selected(mSFMLCanvas, mMainController, selected));
        delete this;

        return true;
    }

    return false;
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

void Selected::keyPressEvent(QKeyEvent * e)
{
    if (e->key() == Qt::Key_Delete) {
        if(!mMainController->isFixed(mQuad.mCell)) {
            mMainController->remove(mQuad, mWireQuad);
            mSFMLCanvas->setState(new Idle(mSFMLCanvas, mMainController));
            delete this;
        }
    }
}

void Selected::mousePressEvent(State::point_type pos)
{
    if(mMainController->hasQuad(pos))
    {
        Quad selected = mMainController->quadAt(pos);

        if (mQuad != selected)
        {
            mMainController->clear(mWireQuad);
            mMainController->mousePress(pos);

            if(mMainController->isFixed(selected.mCell)) {
                mSFMLCanvas->setState(new Selected(mSFMLCanvas, mMainController, selected));
            } else {
                mSFMLCanvas->setState(new Dragging(mSFMLCanvas, mMainController, selected, pos));
            }
            delete this;
        }
        else
        {
            if (!mMainController->isFixed(selected.mCell)) {
                mMainController->clear(mWireQuad);
                mSFMLCanvas->setState(new Dragging(mSFMLCanvas, mMainController, selected, pos));
                delete this;
            }
        }

    } else {
        mMainController->clear(mWireQuad);
        mMainController->mousePress(pos);
        mSFMLCanvas->setState(new Idle(mSFMLCanvas, mMainController));
        delete this;
    }
}

void Selected::mouseReleaseEvent(State::point_type pos)
{
    Quad first = mMainController->quadsCell(mQuad.mCell).front();
    auto origin = mSFMLCanvas->canvas()->points(first).front();

    if (!mMainController->isFixed(first.mCell))
    {
        sf::Vector2f delta(pos.x() - origin.position.x, pos.y() - origin.position.y);
        sf::Transform translation;

        translation.translate(delta.x, delta.y);

        mMainController->transform(mQuad, translation);
        mMainController->transform(mWireQuad, translation);

        origin = mSFMLCanvas->canvas()->points(first).front();
        mMainController->update(mQuad);
        mMainController->mouseMove(State::point_type(origin.position.x, origin.position.y));
    }
}

bool Selected::findCellEvent(QString name)
{
    if(mMainController->hasCell(name.toStdString()))
    {
        Quad selected = mMainController->quadsCell(name.toStdString()).front();

        if (selected.mCell != mQuad.mCell)
        {
            mMainController->clear(mWireQuad);
            mMainController->selectedCell(selected.mCell);
            mSFMLCanvas->setState(new Selected(mSFMLCanvas, mMainController, selected));
            delete this;
        }

        auto origin = mSFMLCanvas->canvas()->points(selected).front();
        auto size = mMainController->cellSize(selected.mCell);
        mSFMLCanvas->centerViewOn(State::point_type(origin.position.x + (size.x() / 2), origin.position.y + (size.y() / 2)));

        return true;
    }

    return false;
}

Dragging::Dragging(MySFMLCanvas * SFMLCanvas, MainController * controller, Quad quad, const State::point_type & pos) :
    State(SFMLCanvas, controller),
    mQuad(quad),
    mWireQuad(controller->wireQuadOfCell(quad.mCell)),
    mInitialPos(pos),
    mMoved(false)
{

}

void Dragging::mouseMoveEvent(State::point_type pos)
{
    sf::Vector2f delta(pos.x() - mInitialPos.x(), pos.y() - mInitialPos.y());
    sf::Transform translation;

    translation.translate(delta.x, delta.y);
    mInitialPos = pos;
    mMoved = true;

    mMainController->transform(mQuad, translation);
    mMainController->transform(mWireQuad, translation);

    Quad first = mMainController->quadsCell(mQuad.mCell).front();
    auto newOrigin = mSFMLCanvas->canvas()->points(first).front();
    mMainController->mouseMove(State::point_type(newOrigin.position.x, newOrigin.position.y));
}

void Dragging::mouseReleaseEvent(State::point_type pos)
{
    mMainController->clear(mWireQuad);
    if (mMoved)
        mMainController->update(mQuad);
    mSFMLCanvas->setState(new Selected(mSFMLCanvas, mMainController, mQuad));
    delete this;
}
