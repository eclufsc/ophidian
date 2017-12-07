#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <vector>
#include <unordered_map>
#include <QObject>

//#include <sstream>
#include <string>

#include <cstdlib>
#include <ctime>
//#include <iostream>
#include <boost/geometry.hpp>
#include <unordered_set>

#include <ophidian/geometry/Models.h>
#include <ophidian/util/Range.h>
#include <ophidian/util/Units.h>
#include <ophidian/design/Design.h>
#include <ophidian/design/DesignBuilder.h>
#include <ophidian/placement/PlacementMapping.h>
#include <ophidian/entity_system/Property.h>

#include "model/forms.h"
#include "view/canvas.h"
#include "control/spatialindex.h"
#include "view/drawablebatch.h"

class MainController : public QObject
{
    Q_OBJECT

    typedef boost::geometry::svg_mapper<ophidian::geometry::Point> SVGMapper;

public:
    //! MainController Constructor
    /*!
       \brief Constructs a MainController with no canvas.
     */
    explicit MainController();

    //! MainController Destructor
    /*!
       \brief Destroys the MainController, including its properties.
     */
    ~MainController();

    //! Canvas setter
    /*!
       \brief Set canvas object for direct use of its functions later.
       \param canvas Canvas pointer.
     */
    void setCanvas(Canvas * canvas);

    //! Selected cell event
    /*!
       \brief Transmits the cell information to the main window.
       \param cell Selected circuit cell.
     */
    void selectedCell(const ophidian::circuit::Cell & cell);

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

    //! Is there a cell with this name?
    /*!
       \brief Verify if a cell with this name exists.
       \param name Cell name.
       \return True if the cell exists, otherwise false.
     */
    bool hasQuad(const ophidian::geometry::Point & p);

    //! Is there a cell in this coordinate?
    /*!
       \brief Verify whether there is a cell in the coordinate.
       \param p A circuit coordinate.
       \return True if the cell is fixed, otherwise false.
     */
    bool hasCell(std::string name);

    //! Quad of a coordinate
    /*!
       \brief Returns the Quad of a given coordinate.
       \param p A circuit coordinate.
       \return The quad corresponding to that position.
     */
    Quad quadAt(const ophidian::geometry::Point & p);

    //! Cell size
    /*!
       \brief Calculates the size of a cell based on all its boxes.
       \param cell Circuit cell.
       \return Point containing the width (x) and height (y).
     */
    ophidian::geometry::Point cellSize(const ophidian::circuit::Cell & cell);

    //! Quads of a cell
    /*!
       \brief Returns the quads of a cell.
       \param cell A circuit cell.
       \return Vector containing all cell quads.
     */
    std::vector<Quad> quadsCell(const ophidian::circuit::Cell & cell);

    //! Quads of a cell
    /*!
       \brief Returns the quads of a cell.
       \param name Cell name.
       \return Vector containing all cell quads.
     */
    std::vector<Quad> quadsCell(std::string name);

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

    //! Draw the containers in svg
    /*!
       \brief Draws all contents of containers in svg.
       \param svg Builder of the SVG image.
       \param viewBox Display area.
     */
    void drawSVG(SVGMapper & mapper, const ophidian::geometry::Box & viewBox);

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
       \param cells_lef Lef file location.
       \param tech_lef Lef file location.
       \param placed_def Def file location.
     */
    void buildICCAD2017(std::string cells_lef, std::string tech_lef, std::string placed_def);

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
    void on_send_circuitChanged(QString name, QString die, size_t cells, size_t pins, size_t nets);

    //! Sending cell information
    /*!
       \brief Sending information from a cell to update the main window.
       \param name Cell name.
       \param type Cell type.
       \param x Position of the origin on the abscissa axis.
       \param y Position of the origin on the ordinate axis.
       \param worstSlack not implemented
     */
    void on_send_cellChanged(QString name, QString type, double width, double height, double x, double y);

private:
    //! Initializes design and cell property to quad.
    /*!
       \brief Helps in abstracting which type of design builder is instantiated for design construction.
     */
    void init();

    //! Canvas upgrade
    /*!
       \brief Redraws circuit cells after executing a slot function.
     */
    void updateAfterSlots();

    //! Creates the quad cells.
    /*!
       \brief Creates all quads of cells and has them draw on canvas.
     */
    void createQuads();

    Canvas * mCanvas;
    SpatialIndex mIndex;
    ophidian::design::Design * mDesign{nullptr};
    ophidian::design::DesignBuilder * mBuilder{nullptr};
    ophidian::entity_system::Property<ophidian::circuit::Cell, std::vector<Quad>> mCellToQuads;
};

#endif // MAINCONTROLLER_H
