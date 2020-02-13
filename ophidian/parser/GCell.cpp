#include "GCell.h"

namespace ophidian::parser
{
    const bool GCell::horizontal() const noexcept
    {
        return m_horizontal;
    }
    
    const GCell::database_unit_type& GCell::origin() const noexcept
    {
        return m_origin;
    }

    const GCell::database_unit_type& GCell::step() const noexcept
    {
        return m_step;
    }

    const GCell::scalar_type& GCell::num() const noexcept
    {
        return m_num;
    }

    bool GCell::operator==(const GCell& rhs) const noexcept
    {
        return m_horizontal == rhs.m_horizontal &&
            m_origin == rhs.m_origin &&
            m_step == rhs.m_step &&
            m_num == rhs.m_num;
    }

    std::ostream& operator<<(std::ostream& os,const GCell& gcell)
    {
        os << "{horizontal: " << gcell.horizontal()
            << ", origin: " << gcell.origin()
            << ", num: " << gcell.num()
            << ", step: " << gcell.step()
            << "}";

        return os;
    }

}
