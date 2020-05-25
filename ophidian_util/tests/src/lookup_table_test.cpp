#include <catch2/catch.hpp>
#include <vector>


#include <ophidian/util/LookupTable.h>


TEST_CASE("lookupTable Floor Test", "[util][lookupTable]")
{
    using namespace ophidian::util;
    using unit_type = double;
    using strategy = FloorStrategy<unit_type, unit_type, unit_type>;
    using table = LookupTable<unit_type, unit_type, unit_type, strategy>;
    using contents = TableContents<unit_type, unit_type, unit_type>;

    contents c;
    c.row_values = {0.0, 0.1, 0.75, 1.5};
    c.column_values = {0.0};
    c.values = {{0.06}, {0.1}, {0.25}, {0.45}};

    table t = table(c);

    auto val = t.compute(0.75, 0.0);

    REQUIRE( val == 0.1);
    REQUIRE( t.row_values().size() == 4);
    REQUIRE( t.column_values().size() == 1);
}

TEST_CASE("lookupTable Ceiling Test", "[util][lookupTable]")
{
    using namespace ophidian::util;
    using unit_type = double;
    using strategy = CeilingStrategy<unit_type, unit_type, unit_type>;
    using table = LookupTable<unit_type, unit_type, unit_type, strategy>;
    using contents = TableContents<unit_type, unit_type, unit_type>;

    contents c;
    c.row_values = {0.0, 0.1, 0.75, 1.5};
    c.column_values = {0.0};
    c.values = {{0.06}, {0.1}, {0.25}, {0.45}};

    table t = table(c);

    auto val = t.compute(0.75, 0.0);

    REQUIRE( val == 0.45);
    REQUIRE( t.row_values().size() == 4);
    REQUIRE( t.column_values().size() == 1);
}
