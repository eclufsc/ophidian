#ifndef RANDOM_PURPLE_CELL_PAINTER_H
#define RANDOM_PURPLE_CELL_PAINTER_H

#include "cell_painter.h"
#include <random>

namespace uddac2016 {

class random_purple_cell_painter : public cell_painter
{
    std::normal_distribution<double> m_distribution;
    std::default_random_engine m_engine;
public:
    random_purple_cell_painter();
    sf::Color color(ophidian::entity::entity cell);

};

}

#endif // RANDOM_PURPLE_CELL_PAINTER_H
