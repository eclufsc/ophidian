#ifndef CELL_PAINTER_H
#define CELL_PAINTER_H

#include <SFML/Graphics.hpp>
#include "../entity/entity.h"

namespace uddac2016 {

class cell_painter
{
public:
    virtual ~cell_painter(){}
    virtual sf::Color color(ophidian::entity::entity cell) = 0;
};

}

#endif // CELL_PAINTER_H
