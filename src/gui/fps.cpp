#include "fps.h"

namespace ophidian {
namespace gui {

fps::fps() :
    m_current(boost::posix_time::microsec_clock::local_time())
{
    m_opensans.loadFromFile("/usr/share/fonts/truetype/open-sans-elementary/OpenSans-Regular.ttf");
    m_text.setColor(sf::Color::Red);
    m_text.setFont(m_opensans);
    m_text.setPosition(30, 30);

}

void fps::update()
{
    m_last = m_current;
    m_current = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration diff = m_current - m_last;
    std::string stdstring{std::to_string(1000./diff.total_milliseconds())};
    stdstring = "FPS: " + stdstring;
    m_text.setString(sf::String(stdstring));
}

void fps::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(m_text);
}

}
}
