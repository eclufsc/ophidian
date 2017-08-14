#include "high_contrast_pallete_color.h"

std::vector<color> high_contrast_pallete_color::m_colors;

high_contrast_pallete_color::high_contrast_pallete_color()
{

}

void high_contrast_pallete_color::initialize_palette_with_20_colors()
{
    m_colors.clear();
    m_colors.reserve(20);
    m_colors.push_back(color(243, 195, 0));
    m_colors.push_back(color(135, 86, 146));
    m_colors.push_back(color(243, 132, 0));
    m_colors.push_back(color(161, 202, 241));
    m_colors.push_back(color(190, 0, 50));
    m_colors.push_back(color(194, 178, 128));
    m_colors.push_back(color(132, 132, 130));
    m_colors.push_back(color(0, 136, 86));
    m_colors.push_back(color(230, 143, 172));
    m_colors.push_back(color(0, 103, 165));
    m_colors.push_back(color(249, 147, 121));
    m_colors.push_back(color(96, 78, 151));
    m_colors.push_back(color(246, 166, 0));
    m_colors.push_back(color(179, 68, 108));
    m_colors.push_back(color(220, 211, 0));
    m_colors.push_back(color(136, 45, 23));
    m_colors.push_back(color(141, 182, 0));
    m_colors.push_back(color(101, 69, 34));
    m_colors.push_back(color(226, 88, 34));
    m_colors.push_back(color(43, 61, 38));
}
