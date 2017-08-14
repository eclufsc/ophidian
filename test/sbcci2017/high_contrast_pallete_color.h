#ifndef HIGH_CONTRAST_PALLETE_COLOR_H
#define HIGH_CONTRAST_PALLETE_COLOR_H

#include <vector>

class color {
public:
    color(unsigned red, unsigned green, unsigned blue)
        : m_red(red), m_green(green), m_blue(blue) {

    }

    unsigned m_red;
    unsigned m_green;
    unsigned m_blue;
};

class high_contrast_pallete_color
{
public:
    high_contrast_pallete_color();

    static void initialize_palette_with_20_colors();

    static std::vector<color> m_colors;
};

#endif // HIGH_CONTRAST_PALLETE_COLOR_H
