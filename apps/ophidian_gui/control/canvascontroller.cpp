#include "canvascontroller.h"

CanvasController::CanvasController(MySFMLCanvas * canvas) :
    mMySFMLCanvas(canvas)
{

}

void CanvasController::mousePress(const ophidian::geometry::Point &p)
{
    //mState->mouse_press(p);
}

void CanvasController::mouseMove(const ophidian::geometry::Point &p)
{
    //mSstate->mouse_move(p);
}

void CanvasController::mouseRelease()
{
    //mSstate->mouse_release();
}
