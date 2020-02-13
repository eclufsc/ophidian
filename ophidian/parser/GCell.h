#ifndef OPHIDIAN_PARSER_GCELL_H
#define OPHIDIAN_PARSER_GCELL_H

#include <ophidian/util/Units.h>

namespace ophidian::parser
{
    /**
     * @brief Type to represent a circuit GCell.
     *
     * This if the data necessary to identify a given
     * row and it's characteristics.
     */
    class GCell
    {
    public:
        using database_unit_type                = util::database_unit_t;
        using scalar_type                       = util::database_unit_scalar_t;

        // Class constructors
        GCell() = delete;

        GCell(const GCell&) = default;
        GCell& operator=(const GCell&) = default;

        GCell(GCell&&) = default;
        GCell& operator=(GCell&&) = default;

        template<class Arg1, class Arg2, class Arg3, class Arg4>
        GCell(Arg1&& horizontal, Arg2&& origin, Arg3&& number, Arg4&& step):
            m_horizontal{std::forward<Arg1>(horizontal)},
            m_origin{std::forward<Arg2>(origin)},
            m_num{std::forward<Arg3>(number)},
            m_step{std::forward<Arg4>(step)}
        {}

        const bool horizontal() const noexcept;

        const database_unit_type& origin() const noexcept;

        const scalar_type& num() const noexcept;

        const database_unit_type& step() const noexcept;

        bool operator==(const GCell& rhs) const noexcept;

        friend std::ostream& operator<<(std::ostream& os, const GCell& gcell);

    private:
        bool m_horizontal;
        database_unit_type m_origin; ///< Specifies the location of the GCell in the circuit.
        scalar_type m_num;  ///< Specifies the number of this GCell type.
        database_unit_type m_step; ///< Specifies the spacing between GCell in horizontal or vertical rows.
    };
}

#endif
