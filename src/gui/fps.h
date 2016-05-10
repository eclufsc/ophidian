#ifndef OPHIDIAN_GUI_FPS_H
#define OPHIDIAN_GUI_FPS_H

#include <SFML/Graphics.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace ophidian {
namespace gui {

class fps : public sf::Drawable
{
    sf::Text m_text;
    sf::Font m_opensans;
    boost::posix_time::ptime m_last;
    boost::posix_time::ptime m_current;
public:
    fps();
    void update();

    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

}
}

#endif // OPHIDIAN_GUI_FPS_H]
