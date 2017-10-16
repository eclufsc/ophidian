#ifndef DRAWPIECES_H
#define DRAWPIECES_H

#include <SFML/Graphics.hpp>
#include <vector>

//#include <ophidian/geometry/Models.h>
//#include "./Model/forms.h"

class DrawPieces : public sf::Drawable
{
public:
    DrawPieces(sf::PrimitiveType primitive);
    ~DrawPieces();

    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    sf::PrimitiveType mPrimitive;
};

#endif // DRAWPIECES_H
