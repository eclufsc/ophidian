#ifndef CANVAS_H
#define CANVAS_H

#include <ophidian/circuit/Netlist.h>
#include "./drawablebatch.h"
#include "./model/forms.h"

class Canvas : public sf::Drawable
{
public:
    using point_type = ophidian::geometry::Point<double>;
    //! Canvas Constructor
    /*!
       \brief Constructs a Canvas
     */
    Canvas();

    //! Canvas Destructor
    /*!
       \brief Destroys the Canvas, including its properties
     */
    virtual ~Canvas();

    //! Clean containers
    /*!
       \brief Cleans containers of quads, lines and boundaries.
     */
    void clear();

    //! Removes the contour of a cell
    /*!
       \brief Removes the corresponding contour of a cell.
       \param wire WireQuad of a cell.
     */
    void clear(WireQuad & wire);

    //! Draw the containers
    /*!
       \brief Draws all contents of containers.
       \param target Render target
       \param states Render states
     */
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    //! Allocates space in container of quads
    /*!
       \brief Reserves memory to allocate all possible amount of quads.
       \param minimumOfQUads Amount of space to reserve.
     */
    void reserveMinimumOfQuads(std::size_t minimumOfQuads);

    /* Quads */
    //! Allocate a quad
    /*!
       \brief Inserts points of a quad into quads container.
       \param quad A quad of the points.
       \param points Vector with points of the quad.
     */
    void alloc(Quad & quad, const std::vector<point_type> & points);

    //! Desalloc quads of a cell
    /*!
       \brief Removes all quads from a cell.
       \param quads Vector with quads of a cell.
     */
    void desalloc(const std::vector<Quad> & quads);

    //! Transform the quads of a cell
    /*!
       \brief Transforms the quads of a cell.
       \param quads Vector with quads of a cell.
       \param trans Transformation to be performed.
     */
    void transform(const std::vector<Quad> & quads, const sf::Transform & trans);

    //! Paints a quad
    /*!
       \brief Paints the quads of a cell.
       \param quads Vector with quads to paiting.
       \param color Painting color.
     */
    void paint(const std::vector<Quad> & quads, const sf::Color color);

    //! Set a point of a quad
    /*!
       \brief Set a point of a quad in a absolute position.
       \param quad The quad.
       \param i Index of the point.
       \param p New point.
     */
    void setPoint(const Quad & quad, std::size_t i, point_type & p);

    //! Vertices of a quad
    /*!
       \brief Returns the vertices of a quad.
       \param quad The quad.
       \return Array containing all points of the quad.
     */
    std::array<sf::Vertex, 4> points(const Quad & quad);

    /* Lines */
    //! Allocate a line
    /*!
       \brief Inserts points of a line into lines container.
       \param line A line of the points.
       \param points Vector with points of the line.
     */
    void alloc(Line & line, const std::vector<point_type> & points);

    //! Desalloc lines
    /*!
       \brief Removes all lines.
       \param lines Vector with lines.
     */
    void desalloc(const std::vector<Line> & lines);

    //! Transform the lines
    /*!
       \brief Transforms the lines.
       \param lines Vector with lines.
       \param trans Transformation to be performed.
     */
    void transform(const std::vector<Line> & lines, const sf::Transform & trans);

    //! Paints a lines
    /*!
       \brief Paints the lines.
       \param lines Vector with lines to paiting.
       \param color Painting color.
     */
    void paint(const std::vector<Line> & lines, const sf::Color color);

    //! Set a point of a line
    /*!
       \brief Set a point of a line in a absolute position.
       \param quad The line.
       \param i Index of the point.
       \param p New point.
     */
    void setPoint(const Line & line, std::size_t i, const point_type p);

    //! Vertices of a line
    /*!
       \brief Returns the vertices of a line.
       \param quad The line.
       \return Array containing all points of the line.
     */
    std::array<sf::Vertex, 2> points(const Line & line);

    /* WireQuads */
    //! Create chip boundaries
    /*!
       \brief Creates a WireQuad to represent the chip boundaries.
       \param chipUpperRightCorner Point of the chip boundaries.
     */
    void createBoundaries(const point_type chipUpperRightCorner);

    //! Create the contour of a cell
    /*!
       \brief Returns the contour of a cell.
       \param cell Circuit cell.
       \param origin Origin point.
       \param size Cell size.
       \return WireQuad where contains the contour of a cell.
     */
    WireQuad createWireQuad(const ophidian::circuit::CellInstance & cell, const point_type & origin, const point_type & size);

private:
    DrawableBatch<4> mQuads;
    DrawableBatch<2> mLines;
    DrawableBatch<2> mBoundaries;
};

#endif // CANVAS_H
