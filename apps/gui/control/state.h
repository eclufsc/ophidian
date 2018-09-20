#ifndef STATE_H
#define STATE_H

#include <QWheelEvent>
#include <QKeyEvent>
#include <ophidian/geometry/Models.h>
#include "control/maincontroller.h"
#include "model/forms.h"

class MySFMLCanvas;

class State
{
public:
    using point_type = ophidian::geometry::Point<double>;
    //! State Constructor
    /*!
       \brief Constructs a State.
       \param SFMLCanvas The canvas that displays the quads.
       \param controller Main Controller.
     */
    State(MySFMLCanvas * SFMLCanvas, MainController * controller);

    //! State Destructor
    /*!
       \brief Destroys the State, including its properties
     */
    ~State();

    //! Update event
    /*!
       \brief Does not update anything.
     */
    virtual void update();

    //! Wheel event
    /*!
       \brief Does not take any action.
       \param e Wheel Event object.
     */
    virtual void wheelEvent(QWheelEvent * e);

    //! Key event
    /*!
       \brief Does not take any action.
       \param e Key Event object.
     */
    virtual void keyPressEvent(QKeyEvent * e);

    //! Mouse press event
    /*!
       \brief Does not take any action.
       \param pos Click position.
     */
    virtual void mousePressEvent(point_type pos);

    //! Mouse move event
    /*!
       \brief Does not take any action.
       \param pos Last mouse position.
     */
    virtual void mouseMoveEvent(point_type pos);

    //! Mouse release event
    /*!
       \brief Does not take any action.
       \param pos Position where the mouse was released.
     */
    virtual void mouseReleaseEvent(point_type pos);

    //! Find cell event
    /*!
       \brief Performs a cell search by name.
       \param e Mouse event object.
       \return True if it encounters a cell, otherwise false.
     */
    virtual bool findCellEvent(QString name);

protected:
    MySFMLCanvas * mSFMLCanvas;
    MainController * mMainController;
};

class Blocked : public State
{
public:
    //! Blocked Constructor
    /*!
       \brief Constructs a Blocked.
       \param SFMLCanvas The canvas that displays the quads.
       \param controller Main Controller.
     */
    Blocked(MySFMLCanvas * SFMLCanvas, MainController * controller);

    //! Blocked Destructor
    /*!
       \brief Destroys the Blocked, including its properties
     */
    ~Blocked();
};

class Idle : public State
{
public:
    using point_type = ophidian::geometry::Point<double>;
    //! Idle Constructor
    /*!
       \brief Constructs a Idle.
       \param SFMLCanvas The canvas that displays the quads.
       \param controller Main Controller.
     */
    Idle(MySFMLCanvas * SFMLCanvas, MainController * controller);

    //! Mouse press event
    /*!
       \brief Verify if there is a cell where it was clicked, then change its state.
       \param pos Click position.
     */
    void mousePressEvent(point_type pos);

    //! Find cell event
    /*!
       \brief Performs a cell search by name.
       \param e Mouse event object.
       \return True if it encounters a cell, otherwise false.
     */
    bool findCellEvent(QString name);
};

class Selected : public State
{
public:
    using point_type = ophidian::geometry::Point<double>;
    //! Selected Constructor
    /*!
       \brief Constructs a Selected.
       \param SFMLCanvas The canvas that displays the quads.
       \param controller Main Controller.
       \param quad Quad of a cell.
     */
    Selected(MySFMLCanvas * SFMLCanvas, MainController * controller, Quad quad);

    //! Selected Destructor
    /*!
       \brief Destroys the Selected, including its properties
     */
    ~Selected();

    //! Key event
    /*!
       \brief Delete key pressed removes the quad cell.
       \param e Key Event object.
     */
    void keyPressEvent(QKeyEvent * e);

    //! Mouse press event
    /*!
       \brief Verify if has been clicked on the same cell, if it is possible to move the cell and then change its state.
       \param pos Click position.
     */
    void mousePressEvent(point_type pos);

    //! Mouse release event
    /*!
       \brief Used to move the cell to an absolute position.
       \param pos Position where the mouse was released.
     */
    virtual void mouseReleaseEvent(point_type pos);

    //! Find cell event
    /*!
       \brief Performs a cell search by name.
       \param e Mouse event object.
       \return True if it encounters a cell, otherwise false.
     */
    bool findCellEvent(QString name);

protected:
    Quad mQuad;
    WireQuad mWireQuad;
};

class Dragging : public State
{
public:
    using point_type = ophidian::geometry::Point<double>;
    //! Selected Constructor
    /*!
       \brief Constructs a Selected.
       \param SFMLCanvas The canvas that displays the quads.
       \param controller Main Controller.
       \param quad Quad of a cell.
       \param pos Origin of the quad.
     */
    Dragging(MySFMLCanvas * SFMLCanvas, MainController * controller, Quad quad, const point_type & pos);

    //! Mouse move event
    /*!
       \brief Moves the quads of the cell.
       \param pos Last mouse position.
     */
    void mouseMoveEvent(point_type pos);

    //! Mouse release event
    /*!
       \brief Removes the contour of the cell, updates the cell position and changes states.
       \param pos Position where the mouse was released.
     */
    void mouseReleaseEvent(point_type pos);

protected:
    Quad mQuad;
    WireQuad mWireQuad;
    point_type mInitialPos;
    bool mMoved;
};

#endif // STATE_H
