#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <vector>
#include <unordered_map>
#include <QObject>

#include <sstream>
#include <string>

#include <cstdlib>
#include <ctime>
#include <iostream>

#include <ophidian/geometry/Models.h>
#include <ophidian/util/Range.h>
#include <ophidian/util/Units.h>
#include <ophidian/design/Design.h>
#include <ophidian/design/DesignBuilder.h>
#include <ophidian/placement/PlacementMapping.h>
#include <ophidian/entity_system/Property.h>

#include "Model/forms.h"
#include "view/canvas.h"
#include "control/spatialindex.h"
#include "view/drawablebatch.h"

class MainController : public QObject
{
    Q_OBJECT

public:
    //! MainController Constructor
    /*!
       \brief Constructs a MainController with no canvas
     */
    explicit MainController();

    //! MainController Destructor
    /*!
       \brief Destroys the MainController, including its properties
     */
    ~MainController();

    //! Canvas setter
    /*!
       \brief Set canvas object for direct use of its functions later.
       \param canvas Canvas pointer.
     */
    void setCanvas(Canvas * canvas);

    //! Mouse press event
    /*!
       \brief Finds cell at corresponding point and transmits its information to main window.
       \param p Point where mouse was clicked.
     */
    void mousePress(const ophidian::geometry::Point & p);

    //! Mouse move event
    /*!
       \brief Updates location information for the main window.
       \param p Point where the mouse is at the current time.
     */
    void mouseMove(const ophidian::geometry::Point & p);

    //! Is the cell fixed?
    /*!
       \brief Verify whether a cell is fixed in the circuit.
       \param cell A circuit cell.
       \return True if the cell is fixed, otherwise false.
     */
    bool isFixed(const ophidian::circuit::Cell & cell);

    //! Is there a cell in this coordinate?
    /*!
       \brief Verify whether there is a cell in the coordinate.
       \param p A circuit coordinate.
       \return True if the cell is fixed, otherwise false.
     */
    bool hasQuad(const ophidian::geometry::Point & p);

    //! Quad of a coordinate
    /*!
       \brief Returns the Quad of a given coordinate.
       \param p A circuit coordinate.
       \return The quad corresponding to that position.
     */
    Quad quadAt(const ophidian::geometry::Point & p);

    //! Quads of a cell
    /*!
       \brief Returns the quads of a cell.
       \param cell A circuit cell.
       \return Vector containing all cell quads.
     */
    std::vector<Quad> quadsCell(const ophidian::circuit::Cell & cell);

    //! Contour of a cell
    /*!
       \brief Returns the contour of a cell.
       \param cell A circuit cell.
       \return WireQuad where contains the contour of a cell.
     */
    WireQuad wireQuadOfCell(const ophidian::circuit::Cell & cell);

    //! Modifies the position of a quad
    /*!
       \brief Move the quad according to the transformation parameters.
       \param quad Quad of a cell.
       \param trans Transformation to be performed.
     */
    void transform(Quad quad, const sf::Transform & trans);

    //! Modifies the position of a wirequad
    /*!
       \brief Move the wirequad according to the transformation parameters.
       \param wire WireQuad of a cell.
       \param trans Transformation to be performed.
     */
    void transform(WireQuad wire, const sf::Transform & trans);

    //! Updates the cell position of a quad
    /*!
       \brief Updates the position of the cell corresponding to the quad.
       \param quad Quad of a cell.
     */
    void update(Quad quad);

    //! Removes a cell
    /*!
       \brief Removes a cell from a quad and wirequad.
       \param quad Quad of a cell.
       \param wire WireQuad of a cell.
     */
    void remove(Quad quad, WireQuad wire);

    //! Removes the contour of a cell
    /*!
       \brief Removes the corresponding contour of a cell.
       \param wire WireQuad of a cell.
     */
    void clear(WireQuad & wire);

    //! Chip Boundaries
    /*!
       \brief Returns the limits of the chip.
       \return Point corresponding to the limits of the chip.
     */
    ophidian::geometry::Point chipBoundaries();

    //! Extra function 1
    /*!
       \brief Function for implementing some extra functionality.
     */
    void slot1();

    //! Extra function 2
    /*!
       \brief Function for implementing some extra functionality.
     */
    void slot2();

    //! Extra function 3
    /*!
       \brief Function for implementing some extra functionality.
     */
    void slot3();

public slots:
    //! Build circuit ICCAD 2017
    /*!
       \brief Using the parameters, find the files and build the circuit.
       \param lef Lef file location.
       \param def Def file location.
       \param verilog Verilog file location.
     */
    void buildICCAD2017(std::string lef, std::string def, std::string verilog);

    //! Build circuit ICCAD 2015
    /*!
       \brief Using the parameters, find the files and build the circuit.
       \param lef Lef file location.
       \param def Def file location.
       \param verilog Verilog file location.
     */
    void buildICCAD2015(std::string lef, std::string def, std::string verilog);

signals:
    //! Sending circuit information
    /*!
       \brief Sending the circuit information to update the main window.
       \param name Circuit name.
       \param die Circuit size.
       \param cells Amount of cells in the circuit.
       \param pins Amount of pins in the circuit.
       \param nets Amount of nets in the circuit.
     */
    void on_circuit_labelsChanged(QString name, QString die, size_t cells, size_t pins, size_t nets);

    //! Sending cell information
    /*!
       \brief Sending information from a cell to update the main window.
       \param name Cell name.
       \param type Cell type.
       \param x Position on the abscissa axis.
       \param y Position on the ordinate axis.
       \param worstSlack not implemented
     */
    void on_selected_cellChanged(QString name, QString type,  double x, double y, int worstSlack);

private:
    //! Creates the quad cells.
    /*!
       \brief Creates all quads of cells and has them draw on canvas.
     */
    void createQuads();

    typedef bool fixed_t;

    Canvas * mCanvas;
    SpatialIndex mIndex;
    ophidian::design::Design * mDesign{nullptr};
    ophidian::design::DesignBuilder * mBuilder{nullptr};
    ophidian::entity_system::Property<ophidian::circuit::Cell, std::pair<std::vector<Quad>, fixed_t>> mCellToQuads;
};

#endif // MAINCONTROLLER_H
