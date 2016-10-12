#include <catch.hpp>
#include <vector>
#include <random>

#include <ophidian/util/range.h>

std::default_random_engine engine;

int generator()
{
    std::uniform_int_distribution<int> dist(0, 100);
    return dist(engine);
}

TEST_CASE("Range Test", "[util][range]")
{
    using Vector = std::vector<int>;
    Vector elements(10);
    std::generate(elements.begin(), elements.end(), generator);
    ophidian::util::Range<Vector::const_iterator> range(elements.begin(), elements.end());
    REQUIRE( std::distance(range.begin(), range.end()) == std::distance(elements.begin(), elements.end()) );
    REQUIRE( range.size() == elements.size() );
    REQUIRE( range.empty() == elements.empty() );
    REQUIRE( std::all_of(range.begin(), range.end(), [](auto el)->bool{return el >= 0 && el <= 100;}) );
}
