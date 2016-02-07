#include "resources.h"

sf::Font resources::m_font;

void resources::load_resources(){
    if(!resources::m_font.loadFromFile("fonts/sourcesans/SourceSansPro-ExtraLight.ttf"))
        throw std::runtime_error("cannot load font fonts/sourcesans/SourceSansPro-ExtraLight.ttf");
}

const sf::Font& resources::font(){
    return resources::m_font;
}
