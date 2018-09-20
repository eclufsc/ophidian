#ifndef MYSFMLCANVAS_H
#define MYSFMLCANVAS_H

#include <omp.h>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <boost/geometry.hpp>

#include <QKeyEvent>
#include <QWheelEvent>
#include <QResizeEvent>

#include <ophidian/geometry/Models.h>
#include "qsfmlcanvas.h"
#include "canvas.h"
#include "control/maincontroller.h"
#include "control/state.h"

using namespace qsfml;

class State;

class MySFMLCanvas : public QSFMLCanvas
{
public:
    using point_type = ophidian::geometry::Point<double>;
    //! MySFMLCanvas Constructor
    /*!
       \brief Constructs a MySFMLCanvas.
     */
    MySFMLCanvas(QWidget * parent = 0);

    //! MySFMLCanvas Destructor
    /*!
       \brief Destroys the MySFMLCanvas, including its properties.
     */
    virtual ~MySFMLCanvas();

    //! MainController setter
    /*!
       \brief Set MainController object for direct use of its functions later.
       \param controller MainController reference.
     */
    void setController(MainController & controller);

    //! Canvas Getter
    /*!
       \brief Get the canvas responsible for the drawings of the cells.
       \return Canvas pointer.
     */
    Canvas * canvas();

    //! Init event
    /*!
       \brief Does not update anything.
     */
    void OnInit();

    //! Update event
    /*!
       \brief Updates canvas and camera view.
     */
    void OnUpdate();

    //! Wheel event
    /*!
       \brief Zoom in or zoom out on canvas.
       \param e Wheel Event object.
     */
    void wheelEvent(QWheelEvent * e);

    //! Resize event
    /*!
       \brief Resets window size.
       \param e Resize Event object.
     */
    void resizeEvent(QResizeEvent * e);

    //! Key event
    /*!
       \brief Move camera view or delete a cell.
       \param e Key Event object.
     */
    void keyPressEvent(QKeyEvent * e);

    //! Mouse press event
    /*!
       \brief Calls the mouse press event of the current state.
       \param e Mouse event object.
     */
    void mousePressEvent(QMouseEvent * e);

    //! Mouse move event
    /*!
       \brief Calls the mouse move event of the current state.
       \param e Mouse event object.
     */
    void mouseMoveEvent(QMouseEvent * e);

    //! Mouse release event
    /*!
       \brief Calls the mouse release event of the current state.
       \param e Mouse event object.
     */
    void mouseReleaseEvent(QMouseEvent * e);

    //! Find cell event
    /*!
       \brief Performs a cell search by name.
       \param e Mouse event object.
       \return True if it encounters a cell, otherwise false.
     */
    bool findCellEvent(QString name);

    //! Updates camera view center
    /*!
       \brief Updates the central coordinate of the camera view.
       \param pos New center position.
     */
    void centerViewOn(const point_type & p1);

    //! Updates camera view size
    /*!
       \brief Updates the size of the camera view.
       \param pos New center position.
     */
    void viewSize(const point_type & size);

    //! Save the circuit in image with PNG format
    /*!
       \brief Saves the current state of the circuit in an image with PNG format.
       \param filename File name and location where the image should be saved.
     */
    void saveToPNG(const std::string & filename);

    //! Save the circuit in image with SVG format
    /*!
       \brief Saves the current state of the circuit in an image with SCG format.
       \param filename File name and location where the image should be saved.
     */
    void saveToSVG(const std::string & filename);

public slots:
    //! State setter
    /*!
       \brief Change current state.
       \param state New state.
     */
    void setState(State * state);

    //! Updates camera view size
    /*!
       \brief Updates the size of the camera view.
       \param state New state.
     */
    void setSize(point_type size);

    //! Allocates space in canvas for the cells
    /*!
       \brief Reserves memory to allocate all possible amount of cells.
       \param minimumOfForms Amount of space to reserve.
     */
    void reserveMinimumOfQuads(std::size_t minimumOfQuads);

    //! Update quad position
    /*!
       \brief Updates the position of the quad, that is, the cell associated with the quad.
       \param p New quad position.
     */
    void updatePositionQuad(const point_type & p);

private:
    //! Convert mouse event to point
    /*!
       \brief Get the point where the event occurred.
       \param pos Click ou move position.
     */
    point_type mouseEventToPoint(QMouseEvent * e);

    typedef boost::geometry::svg_mapper<point_type> SVGMapper;

    MainController * mMainController;
    State * mState;
    Canvas mCanvas;
    sf::View mCameraView;
};

#endif // MYSFMLCANVAS_H
