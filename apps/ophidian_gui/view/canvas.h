#ifndef CANVAS_H
#define CANVAS_H

#include <ophidian/circuit/Netlist.h>
#include "./drawablebatch.h"
#include "./Model/forms.h"

class Canvas : public sf::Drawable
{
public:
    Canvas();
    virtual ~Canvas();

    void clear();
    void clear(WireQuad & wire);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    void reserveMinimumOfQuads(std::size_t minimumOfQuads);

    DrawableBatch<4> * drawableQuads();
    DrawableBatch<2> * drawableLines();

    /* Quads */
    void alloc(Quad & quad, const std::vector<ophidian::geometry::Point> & points);
    void transform(Quad, const std::vector<Form> & quads, const sf::Transform & trans);
    void paint(Quad, const std::vector<Form> & quads, const sf::Color color);
    void setPoint(const Quad & quad, std::size_t i, ophidian::geometry::Point & p);
    std::array<sf::Vertex, 4> points(const Quad & quad);

    /* Lines */
    void alloc(Line & line, const std::vector<ophidian::geometry::Point> & points);
    void transform(Line, const std::vector<Form> & lines, const sf::Transform & trans);
    void paint(Line, const std::vector<Form> & lines, const sf::Color color);
    void setPoint(const Line & line, std::size_t i, ophidian::geometry::Point & p);
    std::array<sf::Vertex, 2> points(const Line & line);

    /* WireQuads */
    WireQuad createWireQuad(const std::vector<Quad> & quads);




    /*
    void allocQuad(Quad & quad, const ophidian::geometry::Point & p1, const ophidian::geometry::Point & p2, const ophidian::geometry::Point & p3, const ophidian::geometry::Point & p4);
    void quadUpdate(Quad & quad, const ophidian::geometry::Point & p1, const ophidian::geometry::Point & p2, const ophidian::geometry::Point & p3, const ophidian::geometry::Point & p4);


    Quad quad_create(const ophidian::geometry::Point & p1, const ophidian::geometry::Point & p2, const ophidian::geometry::Point & p3, const ophidian::geometry::Point & p4);
    void destroy(Quad quad);
    void transform(Quad quad, const sf::Transform & transformation);
    void paint(Quad quad, sf::Color color);

    void quad_update(Quad quad, const ophidian::geometry::Point & p1, const ophidian::geometry::Point & p2, const ophidian::geometry::Point & p3, const ophidian::geometry::Point & p4);

    // std::array<sf::Vertex, 4> & quad_points(Quad quad) const {
    //    return mQuads.points(quad.cell);
    //}

    //<4>& quads() {
    //    return mQuads;
    //}

    void update();
    void clear();*/

private:
    DrawableBatch<4> mQuads;
    DrawableBatch<2> mLines;
};

#endif // CANVAS_H
