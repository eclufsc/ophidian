#include "liberty_test.h"
#include <catch.hpp>

#include <ophidian/parser/LibertyParser.h>
#include <istream>
#include <fstream>
using namespace ophidian::parser;

TEST_CASE("Liberty: init", "[parser][LibertyParser]")
{
    LibertyParser libertyParser = LibertyParser();

}
