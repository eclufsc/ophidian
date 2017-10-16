#ifndef CANVAS_H
#define CANVAS_H

#include <ophidian/circuit/Netlist.h>
#include "./drawablebatch.h"
#include "./Model/forms.h"

#include "drawpieces.h"

class Canvas : public sf::Drawable
{
public:
    Canvas();
    virtual ~Canvas();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;


    /*
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
    //DrawableBatch<4> mQuads{sf::Quads};
    DrawPieces mQuads{sf::Quads};
};

#endif // CANVAS_H
