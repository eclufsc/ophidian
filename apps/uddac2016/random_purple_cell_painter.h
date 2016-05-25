#ifndef RANDOM_PURPLE_CELL_PAINTER_H
#define RANDOM_PURPLE_CELL_PAINTER_H

#include "cell_painter.h"
#include <random>

#include <unordered_map>

namespace uddac2016 {

class random_purple_cell_painter : public cell_painter
{
    std::function<bool(const ophidian::entity_system::entity&)> m_is_fixed;

    std::normal_distribution<double> m_distribution;
    std::default_random_engine m_engine;
public:
    random_purple_cell_painter(std::function<bool(const ophidian::entity_system::entity&)> is_fixed);
    sf::Color color(ophidian::entity_system::entity cell);

};


class range_painter : public cell_painter {
    const std::unordered_map<ophidian::entity_system::entity, std::pair<double, bool> > * m_color_map{nullptr};
    std::pair<sf::Color, sf::Color> m_negative;
    sf::Color m_zero;
    sf::Color m_positive;
    sf::Color m_default{120,120,120};
    std::pair<double, double> m_range;

public:
    void negative(const std::pair<const sf::Color, const sf::Color> & neg);
    void zero(const sf::Color & color);
    void positive(const sf::Color & color);
    void default_(const sf::Color & color);
    void color_map(const std::unordered_map<ophidian::entity_system::entity, std::pair<double, bool> >& map);
    sf::Color color(ophidian::entity_system::entity cell);
};

}

#endif // RANDOM_PURPLE_CELL_PAINTER_H
