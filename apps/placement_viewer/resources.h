#ifndef RESOURCES
#define RESOURCES

#include <SFML/Graphics.hpp>

class resources {

    static sf::Font m_font;

public:
    static void load_resources();
    static const sf::Font &font();
};


#endif // RESOURCES

